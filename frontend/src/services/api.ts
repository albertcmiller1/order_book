import axios from 'axios';
import { Order, Stock, Spread, ChartData, OrderBook } from '../types';

// Replace with your backend URL
const API_URL = 'http://localhost:8000';

// Create axios instance with base URL
const api = axios.create({
  baseURL: API_URL,
  headers: {
    'Content-Type': 'application/json'
  }
});

// Order endpoints
export const getOrders = (userId: string): Promise<Order[]> => {
  return api.get(`/orders?userId=${userId}`).then(response => response.data);
};

export const getOrder = (orderId: string): Promise<Order> => {
  return api.get(`/orders/${orderId}`).then(response => response.data);
};

export const placeOrder = (order: Omit<Order, 'id' | 'status' | 'timestamp' | 'filledQuantity'>): Promise<Order> => {
  return api.post('/orders', order).then(response => response.data);
};

export const cancelOrder = (orderId: string): Promise<Order> => {
  return api.delete(`/orders/${orderId}`).then(response => response.data);
};

// Stock endpoints
export const getStocks = (): Promise<Stock[]> => {
  return api.get('/stocks').then(response => response.data);
};

export const addStock = (stock: Omit<Stock, 'id'>): Promise<Stock> => {
  return api.post('/stocks', stock).then(response => response.data);
};

// Market data endpoints
export const getSpread = (symbol: string): Promise<Spread> => {
  return api.get(`/market/${symbol}/spread`).then(response => response.data);
};

export const getOrderBook = (symbol: string): Promise<OrderBook> => {
  return api.get(`/market/${symbol}/book`).then(response => response.data);
};

export const getChartData = (symbol: string, timeframe: string = '1d'): Promise<ChartData[]> => {
  return api.get(`/market/${symbol}/chart?timeframe=${timeframe}`).then(response => response.data);
};

export default api;