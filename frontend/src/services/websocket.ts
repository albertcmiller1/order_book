import { Spread, OrderBook } from '../types';

// WebSocket URL for FastAPI
const WS_URL = 'ws://127.0.0.1:8000/ws/orderbook';

class WebSocketService {
  private socket: WebSocket | null = null;
  private spreadSubscribers: Map<string, ((spread: Spread) => void)[]> = new Map();
  private orderBookSubscribers: Map<string, ((orderBook: OrderBook) => void)[]> = new Map();
  private connected: boolean = false;
  private reconnectTimer: NodeJS.Timeout | null = null;
  private reconnectAttempts: number = 0;
  private maxReconnectAttempts: number = 5;

  connect(): Promise<void> {
    return new Promise((resolve, reject) => {
      if (this.socket && (this.socket.readyState === WebSocket.OPEN || this.socket.readyState === WebSocket.CONNECTING)) {
        resolve();
        return;
      }

      try {
        console.log('Connecting to WebSocket server...');
        this.socket = new WebSocket(WS_URL);

        this.socket.onopen = () => {
          console.log('WebSocket connected successfully');
          this.connected = true;
          this.reconnectAttempts = 0;
          resolve();
        };

        this.socket.onerror = (error) => {
          console.error('WebSocket connection error:', error);
          if (!this.connected) {
            reject(error);
          }
          this.attemptReconnect();
        };

        this.socket.onmessage = (event) => {
          try {
            const data = JSON.parse(event.data);
            
            // Handle spread updates
            if (data.symbol && data.bid !== undefined && data.ask !== undefined) {
              const spread: Spread = {
                symbol: data.symbol,
                bid: data.bid,
                ask: data.ask,
                lastTradePrice: data.lastTradePrice,
                lastTradeTime: data.lastTradeTime
              };
              
              const subscribers = this.spreadSubscribers.get(spread.symbol) || [];
              subscribers.forEach(callback => callback(spread));
            }
            
            // Handle orderbook updates (if they come in the future)
            if (data.symbol && data.bids && data.asks) {
              const orderBook: OrderBook = {
                symbol: data.symbol,
                bids: data.bids,
                asks: data.asks,
                timestamp: data.timestamp
              };
              
              const subscribers = this.orderBookSubscribers.get(orderBook.symbol) || [];
              subscribers.forEach(callback => callback(orderBook));
            }
          } catch (err) {
            console.error('Error parsing WebSocket message:', err);
          }
        };

        this.socket.onclose = (event) => {
          console.log(`WebSocket disconnected: ${event.code} ${event.reason}`);
          this.connected = false;
          this.attemptReconnect();
        };
      } catch (err) {
        console.error('Error creating WebSocket:', err);
        reject(err);
      }
    });
  }

  private attemptReconnect() {
    if (this.reconnectTimer || this.reconnectAttempts >= this.maxReconnectAttempts) {
      return;
    }

    this.reconnectAttempts++;
    console.log(`Attempting to reconnect (${this.reconnectAttempts}/${this.maxReconnectAttempts})...`);
    
    this.reconnectTimer = setTimeout(() => {
      this.reconnectTimer = null;
      this.connect().catch(() => {
        // Reconnect attempt failed, will try again if not max attempts
      });
    }, 2000); // Retry every 2 seconds
  }

  disconnect(): void {
    if (this.socket) {
      this.socket.close();
      this.socket = null;
    }
    this.connected = false;
    
    if (this.reconnectTimer) {
      clearTimeout(this.reconnectTimer);
      this.reconnectTimer = null;
    }
  }

  subscribeToSpread(symbol: string, callback: (spread: Spread) => void): void {
    if (!this.spreadSubscribers.has(symbol)) {
      this.spreadSubscribers.set(symbol, []);
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
      } else {
        this.spreadSubscribers.set(symbol, filtered);
      }
    } else {
      this.spreadSubscribers.delete(symbol);
    }
  }

  subscribeToOrderBook(symbol: string, callback: (orderBook: OrderBook) => void): void {
    if (!this.orderBookSubscribers.has(symbol)) {
      this.orderBookSubscribers.set(symbol, []);
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
      } else {
        this.orderBookSubscribers.set(symbol, filtered);
      }
    } else {
      this.orderBookSubscribers.delete(symbol);
    }
  }
}

const websocketService = new WebSocketService();
export default websocketService;