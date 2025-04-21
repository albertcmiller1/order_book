import React, { useEffect, useState } from 'react';
import { Paper, Typography, Box, Chip, CircularProgress } from '@mui/material';
import { Spread } from '../types';
import { getSpread } from '../services/api';
import websocketService from '../services/websocket';

interface SpreadDisplayProps {
  symbol: string;
}

const SpreadDisplay: React.FC<SpreadDisplayProps> = ({ symbol }) => {
  const [spread, setSpread] = useState<Spread | null>(null);
  const [loading, setLoading] = useState<boolean>(true);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    const fetchSpread = async () => {
      setLoading(true);
      setError(null);
      
      try {
        const data = await getSpread(symbol);
        setSpread(data);
      } catch (err) {
        console.error('Error fetching spread:', err);
        setError('Failed to load spread data');
      } finally {
        setLoading(false);
      }
    };

    fetchSpread();

    // Subscribe to spread updates via WebSocket
    const handleSpreadUpdate = (data: Spread) => {
      setSpread(data);
    };

    websocketService.connect().then(() => {
      websocketService.subscribeToSpread(symbol, handleSpreadUpdate);
    }).catch(err => {
      console.error('WebSocket connection error:', err);
    });
    
    return () => {
      websocketService.unsubscribeFromSpread(symbol, handleSpreadUpdate);
    };
  }, [symbol]);

  if (loading) {
    return (
      <Paper sx={{ p: 2, display: 'flex', justifyContent: 'center', alignItems: 'center', minHeight: 100 }}>
        <CircularProgress size={24} />
      </Paper>
    );
  }

  if (error) {
    return (
      <Paper sx={{ p: 2, display: 'flex', justifyContent: 'center', alignItems: 'center', minHeight: 100 }}>
        <Typography color="error">{error}</Typography>
      </Paper>
    );
  }

  if (!spread) {
    return (
      <Paper sx={{ p: 2, display: 'flex', justifyContent: 'center', alignItems: 'center', minHeight: 100 }}>
        <Typography>No spread data available</Typography>
      </Paper>
    );
  }

  return (
    <Paper sx={{ p: 2 }}>
      <Typography variant="h6" gutterBottom>
        {symbol} Spread
      </Typography>
      <Box display="flex" justifyContent="space-around" alignItems="center" py={1}>
        <Box textAlign="center">
          <Typography variant="body2" color="text.secondary">
            Bid
          </Typography>
          <Chip 
            label={spread.bid.toFixed(2)} 
            color="success" 
            sx={{ fontSize: '1.2rem', fontWeight: 'bold', minWidth: 80 }}
          />
        </Box>
        <Box textAlign="center">
          <Typography variant="body2" color="text.secondary">
            Spread
          </Typography>
          <Typography variant="body1" sx={{ fontWeight: 'bold' }}>
            {(spread.ask - spread.bid).toFixed(2)}
          </Typography>
        </Box>
        <Box textAlign="center">
          <Typography variant="body2" color="text.secondary">
            Ask
          </Typography>
          <Chip 
            label={spread.ask.toFixed(2)} 
            color="error" 
            sx={{ fontSize: '1.2rem', fontWeight: 'bold', minWidth: 80 }}
          />
        </Box>
      </Box>
      {spread.lastTradePrice && (
        <Box mt={2} display="flex" justifyContent="center" alignItems="center">
          <Typography variant="body2" color="text.secondary" mr={1}>
            Last Trade:
          </Typography>
          <Typography variant="body1" sx={{ fontWeight: 'bold' }}>
            {spread.lastTradePrice.toFixed(2)}
          </Typography>
          {spread.lastTradeTime && (
            <Typography variant="caption" color="text.secondary" ml={1}>
              at {new Date(spread.lastTradeTime).toLocaleTimeString()}
            </Typography>
          )}
        </Box>
      )}
    </Paper>
  );
};

export default SpreadDisplay;