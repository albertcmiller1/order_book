import React, { useState } from 'react';
import { Button, TextField, FormControl, InputLabel, Select, MenuItem, Paper, Typography } from '@mui/material';
import { Grid } from '@mui/material';
import { Order } from '../types';
import { placeOrder } from '../services/api';

interface OrderFormProps {
  userId: string;
  symbol: string;
  onOrderPlaced?: (order: Order) => void;
}

const OrderForm: React.FC<OrderFormProps> = ({ userId, symbol, onOrderPlaced }) => {
  const [side, setSide] = useState<'BUY' | 'SELL'>('BUY');
  const [price, setPrice] = useState<string>('');
  const [quantity, setQuantity] = useState<string>('');
  const [isSubmitting, setIsSubmitting] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    
    if (!price || !quantity) {
      setError('Please fill in all fields');
      return;
    }

    setIsSubmitting(true);
    setError(null);

    try {
      const newOrder = await placeOrder({
        userId,
        symbol,
        side,
        price: parseFloat(price),
        quantity: parseInt(quantity, 10)
      });
      
      if (onOrderPlaced) {
        onOrderPlaced(newOrder);
      }
      
      // Reset form
      setPrice('');
      setQuantity('');
    } catch (err) {
      setError('Failed to place order. Please try again.');
      console.error('Error placing order:', err);
    } finally {
      setIsSubmitting(false);
    }
  };

  return (
    <Paper sx={{ p: 2 }}>
      <Typography variant="h6" gutterBottom>
        Place Order
      </Typography>
      <form onSubmit={handleSubmit}>
        <Grid container spacing={2}>
          <Grid item xs={12}>
            <FormControl fullWidth>
              <InputLabel>Side</InputLabel>
              <Select
                value={side}
                label="Side"
                onChange={(e) => setSide(e.target.value as 'BUY' | 'SELL')}
              >
                <MenuItem value="BUY">Buy</MenuItem>
                <MenuItem value="SELL">Sell</MenuItem>
              </Select>
            </FormControl>
          </Grid>
          <Grid item xs={12}>
            <TextField
              label="Price"
              type="number"
              fullWidth
              value={price}
              onChange={(e) => setPrice(e.target.value)}
              inputProps={{ step: 0.01, min: 0.01 }}
              required
            />
          </Grid>
          <Grid item xs={12}>
            <TextField
              label="Quantity"
              type="number"
              fullWidth
              value={quantity}
              onChange={(e) => setQuantity(e.target.value)}
              inputProps={{ step: 1, min: 1 }}
              required
            />
          </Grid>
          {error && (
            <Grid item xs={12}>
              <Typography color="error">{error}</Typography>
            </Grid>
          )}
          <Grid item xs={12}>
            <Button
              type="submit"
              variant="contained"
              color={side === 'BUY' ? 'success' : 'error'}
              fullWidth
              disabled={isSubmitting}
            >
              {isSubmitting ? 'Submitting...' : `Place ${side} Order`}
            </Button>
          </Grid>
        </Grid>
      </form>
    </Paper>
  );
};

export default OrderForm;