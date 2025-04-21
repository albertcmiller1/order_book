import React, { useEffect, useState } from 'react';
import { 
  Table, TableBody, TableCell, TableContainer, TableHead, TableRow,
  Paper, Typography, Box, CircularProgress 
} from '@mui/material';
import { Grid } from '@mui/material';
import { OrderBook as OrderBookType, OrderBookEntry } from '../types';
import { getOrderBook } from '../services/api';
import websocketService from '../services/websocket';

interface OrderBookComponentProps {
  symbol: string;
}

const OrderBookComponent: React.FC<OrderBookComponentProps> = ({ symbol }) => {
  const [orderBook, setOrderBook] = useState<OrderBookType | null>(null);
  const [loading, setLoading] = useState<boolean>(true);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    const fetchOrderBook = async () => {
      setLoading(true);
      setError(null);
      
      try {
        const data = await getOrderBook(symbol);
        setOrderBook(data);
      } catch (err) {
        console.error('Error fetching order book:', err);
        setError('Failed to load order book');
      } finally {
        setLoading(false);
      }
    };

    fetchOrderBook();

    // Subscribe to order book updates via WebSocket
    const handleOrderBookUpdate = (data: OrderBookType) => {
      setOrderBook(data);
    };

    websocketService.connect().then(() => {
      websocketService.subscribeToOrderBook(symbol, handleOrderBookUpdate);
    }).catch(err => {
      console.error('WebSocket connection error:', err);
    });
    
    return () => {
      websocketService.unsubscribeFromOrderBook(symbol, handleOrderBookUpdate);
    };
  }, [symbol]);

  if (loading) {
    return (
      <Paper sx={{ p: 2, display: 'flex', justifyContent: 'center', alignItems: 'center', height: 300 }}>
        <CircularProgress />
      </Paper>
    );
  }

  if (error) {
    return (
      <Paper sx={{ p: 2, display: 'flex', justifyContent: 'center', alignItems: 'center', height: 300 }}>
        <Typography color="error">{error}</Typography>
      </Paper>
    );
  }

  if (!orderBook) {
    return (
      <Paper sx={{ p: 2, display: 'flex', justifyContent: 'center', alignItems: 'center', height: 300 }}>
        <Typography>No order book data available</Typography>
      </Paper>
    );
  }

  return (
    <Paper sx={{ p: 2 }}>
      <Typography variant="h6" gutterBottom>
        {symbol} Order Book
      </Typography>
      <Grid container spacing={2}>
        <Grid item xs={6}>
          <Typography variant="subtitle1" align="center" sx={{ fontWeight: 'bold' }}>
            Bids
          </Typography>
          <TableContainer component={Paper} variant="outlined">
            <Table size="small">
              <TableHead>
                <TableRow>
                  <TableCell>Price</TableCell>
                  <TableCell align="right">Quantity</TableCell>
                </TableRow>
              </TableHead>
              <TableBody>
                {orderBook.bids.length > 0 ? (
                  orderBook.bids.map((bid: OrderBookEntry, index) => (
                    <TableRow key={`bid-${index}`}>
                      <TableCell sx={{ color: 'success.main' }}>{bid.price.toFixed(2)}</TableCell>
                      <TableCell align="right">{bid.quantity}</TableCell>
                    </TableRow>
                  ))
                ) : (
                  <TableRow>
                    <TableCell colSpan={2} align="center">No bids</TableCell>
                  </TableRow>
                )}
              </TableBody>
            </Table>
          </TableContainer>
        </Grid>
        <Grid item xs={6}>
          <Typography variant="subtitle1" align="center" sx={{ fontWeight: 'bold' }}>
            Asks
          </Typography>
          <TableContainer component={Paper} variant="outlined">
            <Table size="small">
              <TableHead>
                <TableRow>
                  <TableCell>Price</TableCell>
                  <TableCell align="right">Quantity</TableCell>
                </TableRow>
              </TableHead>
              <TableBody>
                {orderBook.asks.length > 0 ? (
                  orderBook.asks.map((ask: OrderBookEntry, index) => (
                    <TableRow key={`ask-${index}`}>
                      <TableCell sx={{ color: 'error.main' }}>{ask.price.toFixed(2)}</TableCell>
                      <TableCell align="right">{ask.quantity}</TableCell>
                    </TableRow>
                  ))
                ) : (
                  <TableRow>
                    <TableCell colSpan={2} align="center">No asks</TableCell>
                  </TableRow>
                )}
              </TableBody>
            </Table>
          </TableContainer>
        </Grid>
      </Grid>
      <Box mt={1} display="flex" justifyContent="flex-end">
        <Typography variant="caption">
          Last updated: {new Date(orderBook.timestamp).toLocaleTimeString()}
        </Typography>
      </Box>
    </Paper>
  );
};

export default OrderBookComponent;