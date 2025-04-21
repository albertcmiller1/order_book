'''
The frontend should be able to connect to a websocket which streams the spread and stock price history 
The frontend should also be able to call api rounds to place or cancel an order 


need to better undersdand async await 
what happens when 10 clients all call place order at the same time... 
does the program only do one request at a time in order? 
does the program allow both to happen? this could cause issues when processing matches 

'''
import book, time
from src.main.py.api import API
from fastapi import FastAPI, WebSocket
from dataclasses import dataclass, fields
from typing import get_type_hints, Optional, List
from pydantic import BaseModel 

app = FastAPI()
api = API()

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


'''
uvicorn main:app --reload

websocat ws://127.0.0.1:8000/ws/orderbook

curl -X POST http://127.0.0.1:8000/place_order \
  -H "Content-Type: application/json" \
  -d '{"ticker": "tsla", "user": "albert", "dollars": 100, "cents": 50, "side": "bid", "shares": 5}'

  
curl -X POST http://127.0.0.1:8000/cancel_order \
     -H "Content-Type: application/json" \
     -d '{"order_id": "abc123", "ticker": "AAPL"}'
'''


@app.get("/listings", response_model=List[Order])
def get_listings_for_user(user: str):
    return api.books()


async def process(ob: book.OrderBook):
    matches: list[book.Match] = ob.process()
    for match in matches: 
        api.print_match(match)
        # TODO: post in db
    return matches


@app.post("/place_order")
async def place_order(incoming_order: dict):
    order = get_order(Order, incoming_order)
    if not order:
        return {"status": "fail", "reason": "Invalid Order"}
    ob = api.get_book(order.ticker)
    order_id = ob.add_order(
        book.OrderType.bid if order.side=="bid" else book.OrderType.ask, 
        order.user, 
        order.shares, 
        book.Money(order.dollars, order.shares)
    )
    api.print_book(ob)
    matches = await process(ob)
    api.print_book(ob)
    return {"status": "ok", "order_id": order_id}


class CancelOrderRequest(BaseModel):
    order_id: str
    ticker: str

@app.post("/cancel_order")
def cancel_order(payload: CancelOrderRequest):
    return {"you_sent": payload}


@app.get("/orders", response_model=List[Order])
def get_orders_for_user(user: str):
    return [order for order in orders_db if order.user == user]


@app.websocket("/ws/orderbook")
async def websocket_endpoint(websocket: WebSocket):
    # TODO pass in ticker 
    await websocket.accept()
    while True:
        # time.sleep(2)
        # ob = api.get_book("TSLA")
        # api.print_book(ob)

        data = {"spread": 0.02, "bid": 101.0, "ask": 101.02}
        await websocket.send_json(data)