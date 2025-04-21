import React, { useEffect, useState } from 'react';
import { 
  Table, TableBody, TableCell, TableContainer, TableHead, TableRow,
  Paper, Typography, Button, Chip, CircularProgress, Box 
} from '@mui/material';
import { Order } from '../types';
import { getOrders, cancelOrder } from '../services/api';

interface OrdersListProps {
  userId: string;
  symbol?: string;
}

const OrdersList: React.FC<OrdersListProps> = ({ userId, symbol }) => {
  const [orders, setOrders] = useState<Order[]>([]);
  const [loading, setLoading] = useState<boolean>(true);
  const [error, setError] = useState<string | null>(null);

  const fetchOrders = async () => {
    setLoading(true);
    setError(null);
    
    try {
      let userOrders = await getOrders(userId);
      
      // Filter by symbol if provided
      if (symbol) {
        userOrders = userOrders.filter(order => order.symbol === symbol);
      }
      
      // Sort by timestamp (newest first)
      userOrders.sort((a, b) => b.timestamp - a.timestamp);
      
      setOrders(userOrders);
    } catch (err) {
      console.error('Error fetching orders:', err);
      setError('Failed to load orders');
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchOrders();
    
    // Refresh orders every 30 seconds
    const intervalId = setInterval(fetchOrders, 30000);
    
    return () => clearInterval(intervalId);
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [userId, symbol]);

  const handleCancelOrder = async (orderId: string) => {
    try {
      await cancelOrder(orderId);
      fetchOrders(); // Refresh the orders list
    } catch (err) {
      console.error('Error canceling order:', err);
      setError('Failed to cancel order');
    }
  };

  const getStatusChipColor = (status: string) => {
    switch (status) {
      case 'OPEN':
        return 'primary';
      case 'FILLED':
        return 'success';
      case 'CANCELED':
        return 'error';
      default:
        return 'default';
    }
  };

  if (loading && orders.length === 0) {
    return (
      <Paper sx={{ p: 2, display: 'flex', justifyContent: 'center', alignItems: 'center', height: 200 }}>
        <CircularProgress />
      </Paper>
    );
  }

  if (error && orders.length === 0) {
    return (
      <Paper sx={{ p: 2, display: 'flex', justifyContent: 'center', alignItems: 'center', height: 200 }}>
        <Typography color="error">{error}</Typography>
      </Paper>
    );
  }

  if (orders.length === 0) {
    return (
      <Paper sx={{ p: 2, display: 'flex', justifyContent: 'center', alignItems: 'center', height: 200 }}>
        <Typography>No orders found</Typography>
      </Paper>
    );
  }

  return (
    <Paper sx={{ p: 2 }}>
      <Box display="flex" justifyContent="space-between" alignItems="center" mb={2}>
        <Typography variant="h6">
          Your Orders
          {symbol && ` for ${symbol}`}
        </Typography>
        {loading && <CircularProgress size={24} />}
      </Box>
      <TableContainer component={Paper} variant="outlined">
        <Table>
          <TableHead>
            <TableRow>
              <TableCell>Symbol</TableCell>
              <TableCell>Side</TableCell>
              <TableCell>Price</TableCell>
              <TableCell>Quantity</TableCell>
              <TableCell>Filled</TableCell>
              <TableCell>Status</TableCell>
              <TableCell>Time</TableCell>
              <TableCell>Actions</TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {orders.map((order) => (
              <TableRow key={order.id}>
                <TableCell>{order.symbol}</TableCell>
                <TableCell>
                  <Typography 
                    color={order.side === 'BUY' ? 'success.main' : 'error.main'}
                    sx={{ fontWeight: 'bold' }}
                  >
                    {order.side}
                  </Typography>
                </TableCell>
                <TableCell>{order.price.toFixed(2)}</TableCell>
                <TableCell>{order.quantity}</TableCell>
                <TableCell>{order.filledQuantity || 0}</TableCell>
                <TableCell>
                  <Chip 
                    label={order.status} 
                    color={getStatusChipColor(order.status)}
                    size="small"
                  />
                </TableCell>
                <TableCell>
                  {new Date(order.timestamp).toLocaleString()}
                </TableCell>
                <TableCell>
                  {order.status === 'OPEN' && (
                    <Button
                      variant="outlined"
                      color="error"
                      size="small"
                      onClick={() => handleCancelOrder(order.id)}
                    >
                      Cancel
                    </Button>
                  )}
                </TableCell>
              </TableRow>
            ))}
          </TableBody>
        </Table>
      </TableContainer>
      {error && (
        <Typography color="error" sx={{ mt: 2 }}>
          {error}
        </Typography>
      )}
    </Paper>
  );
};

export default OrdersList;