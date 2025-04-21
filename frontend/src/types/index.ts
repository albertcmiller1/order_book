export interface Order {
  id: string;
  userId: string;
  symbol: string;
  side: 'BUY' | 'SELL';
  price: number;
  quantity: number;
  status: 'OPEN' | 'FILLED' | 'CANCELED';
  filledQuantity?: number;
  timestamp: number;
}

export interface Stock {
  symbol: string;
  name: string;
  description?: string;
}

export interface Spread {
  symbol: string;
  bid: number;
  ask: number;
  lastTradePrice?: number;
  lastTradeTime?: number;
}

export interface ChartData {
  time: string;
  price: number;
  volume?: number;
}

export interface OrderBookEntry {
  price: number;
  quantity: number;
}

export interface OrderBook {
  symbol: string;
  bids: OrderBookEntry[];
  asks: OrderBookEntry[];
  timestamp: number;
}