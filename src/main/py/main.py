'''
The frontend should be able to connect to a websocket which streams the spread and stock price history 
The frontend should also be able to call api rounds to place or cancel an order 
'''
import book, time, json, random
from src.main.py.api import OrderBookService
from fastapi import FastAPI, WebSocket, HTTPException, Query
from fastapi.middleware.cors import CORSMiddleware
from dataclasses import dataclass, fields, asdict
from typing import get_type_hints, Optional, List, Dict
from pydantic import BaseModel
import asyncio
from datetime import datetime

app = FastAPI()
obs = OrderBookService()

# Add CORS middleware to allow frontend to connect
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*", "http://localhost:3000"],  # For production, restrict this to your frontend URL
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Sample stocks data
STOCKS = [
    {"symbol": "AAPL", "name": "Apple Inc.", "description": "Consumer electronics and software"},
    {"symbol": "MSFT", "name": "Microsoft Corporation", "description": "Software and cloud services"},
    {"symbol": "TSLA", "name": "Tesla Inc.", "description": "Electric vehicles and clean energy"},
    {"symbol": "AMZN", "name": "Amazon.com Inc.", "description": "E-commerce and cloud computing"},
    {"symbol": "GOOGL", "name": "Alphabet Inc.", "description": "Internet services and products"}
]

# In-memory database for orders
orders_db = []
connected_clients = []

@dataclass
class Order:
    ticker: str
    user: str
    dollars: int
    cents: int
    side: str
    shares: int


def get_order(cls: dataclass, data: dict) -> Optional[Order]:
    if set(data.keys()) != {f.name for f in fields(cls)}:
        return None
    type_hints = get_type_hints(cls)
    for key, expected_type in type_hints.items():
        if not isinstance(data[key], expected_type):
            return None
    order = Order(**data)
    if (
        order.dollars < 0 or 
        order.cents < 0 or 
        order.cents > 99 or 
        order.side not in {"bid", "ask"}
    ): return None
    return order


# Generate some price history for stocks
def generate_chart_data(symbol: str, days=14):
    data = []
    base_price = random.randint(50, 500)
    
    for i in range(days):
        timestamp = int(datetime.now().timestamp()) - (days - i) * 86400
        price = base_price + random.uniform(-20, 20)
        
        data.append({
            "time": datetime.fromtimestamp(timestamp).strftime("%Y-%m-%d"),
            "price": round(price, 2)
        })
        
        # Update base price for the next day
        base_price = price
    
    return data


# Endpoint to get all available stocks
@app.get("/stocks")
async def get_stocks():
    return STOCKS


# Endpoint to get specific stock info
@app.get("/stocks/{symbol}")
async def get_stock(symbol: str):
    for stock in STOCKS:
        if stock["symbol"] == symbol:
            return stock
    raise HTTPException(status_code=404, detail="Stock not found")


@app.get("/market/{symbol}/chart")
async def get_chart_data(symbol: str, timeframe: str = Query("1d")):
    for stock in STOCKS:
        if stock["symbol"] == symbol:
            return generate_chart_data(symbol)
    raise HTTPException(status_code=404, detail="Stock not found")


@app.get("/market/{symbol}/spread")
async def get_spread(symbol: str):
    ob = obs.get_book(symbol)
    # Get or create order book for the symbol
    
    # Get highest bid and lowest ask
    highest_bid = obs.format_money(ob.prominent_limit(book.OrderType.bid)) 
    lowest_ask = obs.format_money(ob.prominent_limit(book.OrderType.ask))
    
    if highest_bid < 0:  # Default values if no orders exist
        highest_bid = random.uniform(90, 100)
        lowest_ask = highest_bid + random.uniform(0.01, 0.05)
    
    return {
        "symbol": symbol,
        "bid": highest_bid,
        "ask": lowest_ask,
        "lastTradePrice": (highest_bid + lowest_ask) / 2,
        "lastTradeTime": int(datetime.now().timestamp())
    }


@app.get("/market/{symbol}/book")
async def get_order_book(symbol: str):
    ob = obs.get_book(symbol)
    
    # Get order book entries
    bid_limits = [obs.format_money(m) for m in ob.get_limits(book.OrderType.bid, 10)]
    ask_limits = [obs.format_money(m) for m in ob.get_limits(book.OrderType.ask, 10)]
    
    bids = []
    asks = []
    
    # If no orders exist, generate some sample data
    if not bid_limits or bid_limits[0] < 0:
        base_price = random.uniform(90, 100)
        for i in range(5):
            price = base_price - i * 0.05
            quantity = random.randint(100, 1000)
            bids.append({"price": round(price, 2), "quantity": quantity})
    else:
        for price in bid_limits:
            if price >= 0:
                bids.append({"price": price, "quantity": random.randint(100, 1000)})
    
    if not ask_limits or ask_limits[0] < 0:
        base_price = bids[0]["price"] + 0.05 if bids else random.uniform(90, 100)
        for i in range(5):
            price = base_price + i * 0.05
            quantity = random.randint(100, 1000)
            asks.append({"price": round(price, 2), "quantity": quantity})
    else:
        for price in ask_limits:
            if price >= 0:
                asks.append({"price": price, "quantity": random.randint(100, 1000)})
    
    return {
        "symbol": symbol,
        "bids": bids,
        "asks": asks,
        "timestamp": int(datetime.now().timestamp())
    }


@app.get("/orders")
async def get_orders_for_user(userId: str):
    # Convert internal order format to the frontend expected format
    frontend_orders = []
    for order in orders_db:
        if order.user == userId:
            price = order.dollars + (order.cents / 100)
            frontend_orders.append({
                "id": f"order-{len(frontend_orders) + 1}",
                "userId": order.user,
                "symbol": order.ticker,
                "side": "BUY" if order.side == "bid" else "SELL",
                "price": price,
                "quantity": order.shares,
                "status": "OPEN",
                "filledQuantity": 0,
                "timestamp": int(datetime.now().timestamp())
            })
    return frontend_orders


async def process(ob: book.OrderBook):
    matches: list[book.Match] = ob.process()
    for match in matches: 
        obs.print_match(match)
        # TODO: post in db
    return matches


@app.post("/orders")
async def place_order(order_data: dict):
    # Map from frontend order format to backend
    incoming_order = {
        "ticker": order_data.get("symbol", ""),
        "user": order_data.get("userId", ""),
        "dollars": int(order_data.get("price", 0)),
        "cents": int((order_data.get("price", 0) % 1) * 100),
        "side": "bid" if order_data.get("side", "") == "BUY" else "ask",
        "shares": order_data.get("quantity", 0)
    }
    
    order = get_order(Order, incoming_order)
    if not order:
        raise HTTPException(status_code=400, detail="Invalid Order")
    
    ob = obs.get_book(order.ticker)
    order_id = ob.add_order(
        book.OrderType.bid if order.side == "bid" else book.OrderType.ask, 
        order.user, 
        order.shares, 
        book.Money(order.dollars, order.cents)
    )
    
    # Add to our in-memory DB
    orders_db.append(order)
    
    obs.print_book(ob)
    matches = await process(ob)
    obs.print_book(ob)
    
    # Broadcast the updated order book to all connected clients
    await broadcast_updates(order.ticker)
    
    # Return the order with frontend format
    return {
        "id": order_id,
        "userId": order.user,
        "symbol": order.ticker,
        "side": "BUY" if order.side == "bid" else "SELL",
        "price": order.dollars + (order.cents / 100),
        "quantity": order.shares,
        "status": "OPEN",
        "filledQuantity": 0,
        "timestamp": int(datetime.now().timestamp())
    }


@app.delete("/orders/{orderId}")
async def cancel_order(orderId: str):
    # In a real implementation, you would find the order and cancel it
    # For now, we'll just return a success response
    return {
        "id": orderId,
        "status": "CANCELED",
        "timestamp": int(datetime.now().timestamp())
    }


async def broadcast_updates(ticker: str):
    """Broadcast updates to all connected clients."""
    if connected_clients:
        # Get spread data
        ob = obs.get_book(ticker)
        highest_bid = obs.format_money(ob.prominent_limit(book.OrderType.bid))
        lowest_ask = obs.format_money(ob.prominent_limit(book.OrderType.ask))
        
        if highest_bid < 0:  # Default values if no orders exist
            highest_bid = random.uniform(90, 100)
            lowest_ask = highest_bid + random.uniform(0.01, 0.05)
        
        spread_data = {
            "symbol": ticker,
            "bid": highest_bid,
            "ask": lowest_ask,
            "lastTradePrice": (highest_bid + lowest_ask) / 2,
            "lastTradeTime": int(datetime.now().timestamp())
        }
        
        for client in connected_clients:
            await client.send_json(spread_data)


@app.websocket("/ws/orderbook")
async def websocket_endpoint(websocket: WebSocket):
    await websocket.accept()
    connected_clients.append(websocket)
    
    try:
        print("New WebSocket client connected")
        # Send initial data for all stocks
        for stock in STOCKS:
            symbol = stock["symbol"]
            data = await get_spread(symbol)
            await websocket.send_json(data)
            print(f"Sent initial data for {symbol}")
            
            # Wait a bit between messages
            await asyncio.sleep(0.2)
        
        # Keep connection alive and send periodic updates
        while True:
            try:
                # Simulate market movements for a random stock
                symbol = random.choice(STOCKS)["symbol"]
                data = await get_spread(symbol)
                
                # Add small price movements
                data["bid"] += random.uniform(-0.02, 0.02)
                data["ask"] += random.uniform(-0.02, 0.02)
                data["lastTradePrice"] = (data["bid"] + data["ask"]) / 2
                data["lastTradeTime"] = int(datetime.now().timestamp())
                
                # Format to 2 decimal places
                data["bid"] = round(data["bid"], 2)
                data["ask"] = round(data["ask"], 2)
                data["lastTradePrice"] = round(data["lastTradePrice"], 2)
                
                await websocket.send_json(data)
                print(f"Sent update for {symbol}: bid={data['bid']}, ask={data['ask']}")
                await asyncio.sleep(2)  # Send update every 2 seconds
            except Exception as inner_e:
                print(f"Error during WebSocket update: {inner_e}")
                if websocket.client_state == WebSocket.DISCONNECTED:
                    break
                await asyncio.sleep(1)
            
    except Exception as e:
        print(f"WebSocket connection error: {e}")
    finally:
        # Remove client on disconnect
        if websocket in connected_clients:
            connected_clients.remove(websocket)
            print("WebSocket client disconnected")

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="127.0.0.1", port=8000)