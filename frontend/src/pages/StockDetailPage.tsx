import React, { useEffect, useState } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import { 
  Container, Typography, Paper, Button, Box,
  CircularProgress, Breadcrumbs, Link
} from '@mui/material';
import Grid from '@mui/material/Grid';
import ArrowBackIcon from '@mui/icons-material/ArrowBack';
import { Stock } from '../types';
import { getStocks } from '../services/api';
import SpreadDisplay from '../components/SpreadDisplay';
import StockChart from '../components/StockChart';
import OrderBook from '../components/OrderBook';
import OrderForm from '../components/OrderForm';
import OrdersList from '../components/OrdersList';

const StockDetailPage: React.FC = () => {
  const { symbol } = useParams<{ symbol: string }>();
  const navigate = useNavigate();
  const [stock, setStock] = useState<Stock | null>(null);
  const [loading, setLoading] = useState<boolean>(true);
  const [error, setError] = useState<string | null>(null);
  
  // Mock user ID (in a real app this would come from auth)
  const userId = "user123";

  useEffect(() => {
    const fetchStockDetails = async () => {
      if (!symbol) return;
      
      setLoading(true);
      setError(null);
      
      try {
        const stocks = await getStocks();
        const foundStock = stocks.find(s => s.symbol === symbol);
        
        if (foundStock) {
          setStock(foundStock);
        } else {
          setError(`Stock ${symbol} not found`);
        }
      } catch (err) {
        console.error('Error fetching stock details:', err);
        setError('Failed to load stock details');
      } finally {
        setLoading(false);
      }
    };

    fetchStockDetails();
  }, [symbol]);

  if (!symbol) {
    return (
      <Container maxWidth="lg" sx={{ mt: 4, mb: 4 }}>
        <Typography variant="h4" gutterBottom>
          Stock not found
        </Typography>
        <Button 
          startIcon={<ArrowBackIcon />}
          onClick={() => navigate('/')}
        >
          Back to Stocks
        </Button>
      </Container>
    );
  }

  if (loading) {
    return (
      <Container maxWidth="lg" sx={{ mt: 4, mb: 4, display: 'flex', justifyContent: 'center' }}>
        <CircularProgress />
      </Container>
    );
  }

  if (error) {
    return (
      <Container maxWidth="lg" sx={{ mt: 4, mb: 4 }}>
        <Typography variant="h4" color="error" gutterBottom>
          {error}
        </Typography>
        <Button 
          startIcon={<ArrowBackIcon />}
          onClick={() => navigate('/')}
        >
          Back to Stocks
        </Button>
      </Container>
    );
  }

  return (
    <Container maxWidth="lg" sx={{ mt: 4, mb: 4 }}>
      <Box mb={3}>
        <Breadcrumbs aria-label="breadcrumb">
          <Link color="inherit" onClick={() => navigate('/')} sx={{ cursor: 'pointer' }}>
            Stocks
          </Link>
          <Typography color="text.primary">{symbol}</Typography>
        </Breadcrumbs>
      </Box>

      <Box mb={3} display="flex" justifyContent="space-between" alignItems="flex-start">
        <Box>
          <Typography variant="h4" gutterBottom>
            {stock?.name || symbol}
          </Typography>
          {stock?.description && (
            <Typography variant="body1" color="text.secondary">
              {stock.description}
            </Typography>
          )}
        </Box>
        <Button 
          startIcon={<ArrowBackIcon />}
          onClick={() => navigate('/')}
          variant="outlined"
        >
          Back to Stocks
        </Button>
      </Box>

      <Grid container spacing={3}>
        <Grid item xs={12} md={8}>
          <StockChart symbol={symbol} />
        </Grid>
        <Grid item xs={12} md={4}>
          <SpreadDisplay symbol={symbol} />
        </Grid>
        <Grid item xs={12} md={8}>
          <OrderBook symbol={symbol} />
        </Grid>
        <Grid item xs={12} md={4}>
          <OrderForm userId={userId} symbol={symbol} />
        </Grid>
        <Grid item xs={12}>
          <OrdersList userId={userId} symbol={symbol} />
        </Grid>
      </Grid>
    </Container>
  );
};

export default StockDetailPage;