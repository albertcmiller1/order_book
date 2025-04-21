import { io, Socket } from 'socket.io-client';
import { Spread, OrderBook } from '../types';

// Replace with your WebSocket URL
const WS_URL = ' ws://127.0.0.1:8000/ws/orderbook';

class WebSocketService {
  private socket: Socket | null = null;
  private spreadSubscribers: Map<string, ((spread: Spread) => void)[]> = new Map();
  private orderBookSubscribers: Map<string, ((orderBook: OrderBook) => void)[]> = new Map();

  connect(): Promise<void> {
    return new Promise((resolve, reject) => {
      this.socket = io(WS_URL);

      this.socket.on('connect', () => {
        console.log('WebSocket connected');
        resolve();
      });

      this.socket.on('connect_error', (error) => {
        console.error('WebSocket connection error:', error);
        reject(error);
      });

      this.socket.on('spread_update', (spread: Spread) => {
        const subscribers = this.spreadSubscribers.get(spread.symbol) || [];
        subscribers.forEach(callback => callback(spread));
      });

      this.socket.on('orderbook_update', (orderBook: OrderBook) => {
        const subscribers = this.orderBookSubscribers.get(orderBook.symbol) || [];
        subscribers.forEach(callback => callback(orderBook));
      });
    });
  }

  disconnect(): void {
    if (this.socket) {
      this.socket.disconnect();
      this.socket = null;
    }
  }

  subscribeToSpread(symbol: string, callback: (spread: Spread) => void): void {
    if (!this.spreadSubscribers.has(symbol)) {
      this.spreadSubscribers.set(symbol, []);
      if (this.socket && this.socket.connected) {
        this.socket.emit('subscribe_spread', { symbol });
      }
    }
    
    const subscribers = this.spreadSubscribers.get(symbol) || [];
    this.spreadSubscribers.set(symbol, [...subscribers, callback]);
  }

  unsubscribeFromSpread(symbol: string, callback?: (spread: Spread) => void): void {
    if (!this.spreadSubscribers.has(symbol)) return;

    if (callback) {
      const subscribers = this.spreadSubscribers.get(symbol) || [];
      const filtered = subscribers.filter(cb => cb !== callback);
      
      if (filtered.length === 0) {
        this.spreadSubscribers.delete(symbol);
        if (this.socket && this.socket.connected) {
          this.socket.emit('unsubscribe_spread', { symbol });
        }
      } else {
        this.spreadSubscribers.set(symbol, filtered);
      }
    } else {
      this.spreadSubscribers.delete(symbol);
      if (this.socket && this.socket.connected) {
        this.socket.emit('unsubscribe_spread', { symbol });
      }
    }
  }

  subscribeToOrderBook(symbol: string, callback: (orderBook: OrderBook) => void): void {
    if (!this.orderBookSubscribers.has(symbol)) {
      this.orderBookSubscribers.set(symbol, []);
      if (this.socket && this.socket.connected) {
        this.socket.emit('subscribe_orderbook', { symbol });
      }
    }
    
    const subscribers = this.orderBookSubscribers.get(symbol) || [];
    this.orderBookSubscribers.set(symbol, [...subscribers, callback]);
  }

  unsubscribeFromOrderBook(symbol: string, callback?: (orderBook: OrderBook) => void): void {
    if (!this.orderBookSubscribers.has(symbol)) return;

    if (callback) {
      const subscribers = this.orderBookSubscribers.get(symbol) || [];
      const filtered = subscribers.filter(cb => cb !== callback);
      
      if (filtered.length === 0) {
        this.orderBookSubscribers.delete(symbol);
        if (this.socket && this.socket.connected) {
          this.socket.emit('unsubscribe_orderbook', { symbol });
        }
      } else {
        this.orderBookSubscribers.set(symbol, filtered);
      }
    } else {
      this.orderBookSubscribers.delete(symbol);
      if (this.socket && this.socket.connected) {
        this.socket.emit('unsubscribe_orderbook', { symbol });
      }
    }
  }
}

const websocketService = new WebSocketService();
export default websocketService;