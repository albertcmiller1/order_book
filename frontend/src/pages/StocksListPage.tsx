import React, { useEffect, useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { 
  Container, Typography, Button, Paper, Box,
  Table, TableBody, TableCell, TableContainer, TableHead, TableRow,
  Chip, CircularProgress, TextField, Dialog, DialogTitle,
  DialogContent, DialogActions
} from '@mui/material';
import { Grid } from '@mui/material';
import AddIcon from '@mui/icons-material/Add';
import { Stock, Spread } from '../types';
import { getStocks, getSpread, addStock } from '../services/api';
import websocketService from '../services/websocket';

const StocksListPage: React.FC = () => {
  const navigate = useNavigate();
  const [stocks, setStocks] = useState<Stock[]>([]);
  const [spreads, setSpreads] = useState<{ [symbol: string]: Spread }>({});
  const [loading, setLoading] = useState<boolean>(true);
  const [error, setError] = useState<string | null>(null);
  const [openAddDialog, setOpenAddDialog] = useState<boolean>(false);
  const [newStock, setNewStock] = useState<{ symbol: string; name: string; description: string }>({
    symbol: '',
    name: '',
    description: ''
  });
  const [addingStock, setAddingStock] = useState<boolean>(false);
  const [addError, setAddError] = useState<string | null>(null);

  const fetchStocks = async () => {
    setLoading(true);
    setError(null);
    
    try {
      const stocksData = await getStocks();
      setStocks(stocksData);
      
      // Fetch initial spread data for each stock
      const spreadsData: { [symbol: string]: Spread } = {};
      await Promise.all(stocksData.map(async (stock) => {
        try {
          const spreadData = await getSpread(stock.symbol);
          spreadsData[stock.symbol] = spreadData;
        } catch (err) {
          console.error(`Error fetching spread for ${stock.symbol}:`, err);
        }
      }));
      
      setSpreads(spreadsData);
    } catch (err) {
      console.error('Error fetching stocks:', err);
      setError('Failed to load stocks');
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchStocks();
  }, []);

  useEffect(() => {
    // Connect to WebSocket for real-time spread updates
    const spreadSubscribers: { [symbol: string]: ((spread: Spread) => void) } = {};
    
    websocketService.connect().then(() => {
      // Subscribe to spread updates for each stock
      stocks.forEach(stock => {
        const handleSpreadUpdate = (spread: Spread) => {
          setSpreads(prev => ({
            ...prev,
            [spread.symbol]: spread
          }));
        };
        
        spreadSubscribers[stock.symbol] = handleSpreadUpdate;
        websocketService.subscribeToSpread(stock.symbol, handleSpreadUpdate);
      });
    }).catch(err => {
      console.error('WebSocket connection error:', err);
    });
    
    return () => {
      // Unsubscribe from all spread updates when component unmounts
      Object.entries(spreadSubscribers).forEach(([symbol, callback]) => {
        websocketService.unsubscribeFromSpread(symbol, callback);
      });
    };
  }, [stocks]);

  const handleAddStock = async () => {
    if (!newStock.symbol || !newStock.name) {
      setAddError('Symbol and Name are required');
      return;
    }
    
    setAddingStock(true);
    setAddError(null);
    
    try {
      await addStock({
        symbol: newStock.symbol.toUpperCase(),
        name: newStock.name,
        description: newStock.description
      });
      
      setOpenAddDialog(false);
      setNewStock({ symbol: '', name: '', description: '' });
      fetchStocks(); // Refresh the stocks list
    } catch (err) {
      console.error('Error adding stock:', err);
      setAddError('Failed to add stock');
    } finally {
      setAddingStock(false);
    }
  };

  const navigateToStock = (symbol: string) => {
    navigate(`/stocks/${symbol}`);
  };

  return (
    <Container maxWidth="lg" sx={{ mt: 4, mb: 4 }}>
      <Box mb={3} display="flex" justifyContent="space-between" alignItems="center">
        <Typography variant="h4">
          Available Stocks
        </Typography>
        <Button 
          variant="contained" 
          startIcon={<AddIcon />}
          onClick={() => setOpenAddDialog(true)}
        >
          Add New Stock
        </Button>
      </Box>

      {loading && stocks.length === 0 ? (
        <Box display="flex" justifyContent="center" mt={4}>
          <CircularProgress />
        </Box>
      ) : error ? (
        <Paper sx={{ p: 3, mt: 2 }}>
          <Typography color="error">{error}</Typography>
        </Paper>
      ) : stocks.length === 0 ? (
        <Paper sx={{ p: 3, mt: 2, textAlign: 'center' }}>
          <Typography variant="h6">No stocks available</Typography>
          <Typography variant="body2" color="text.secondary" mt={1}>
            Click "Add New Stock" to list your first stock
          </Typography>
        </Paper>
      ) : (
        <TableContainer component={Paper}>
          <Table>
            <TableHead>
              <TableRow>
                <TableCell>Symbol</TableCell>
                <TableCell>Name</TableCell>
                <TableCell>Bid</TableCell>
                <TableCell>Ask</TableCell>
                <TableCell>Spread</TableCell>
                <TableCell>Last Trade</TableCell>
                <TableCell>Action</TableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {stocks.map((stock) => {
                const spread = spreads[stock.symbol];
                return (
                  <TableRow 
                    key={stock.symbol}
                    hover
                    onClick={() => navigateToStock(stock.symbol)}
                    sx={{ cursor: 'pointer' }}
                  >
                    <TableCell>
                      <Typography sx={{ fontWeight: 'bold' }}>
                        {stock.symbol}
                      </Typography>
                    </TableCell>
                    <TableCell>{stock.name}</TableCell>
                    <TableCell>
                      {spread ? (
                        <Typography color="success.main">
                          {spread.bid.toFixed(2)}
                        </Typography>
                      ) : (
                        <Chip label="N/A" size="small" />
                      )}
                    </TableCell>
                    <TableCell>
                      {spread ? (
                        <Typography color="error.main">
                          {spread.ask.toFixed(2)}
                        </Typography>
                      ) : (
                        <Chip label="N/A" size="small" />
                      )}
                    </TableCell>
                    <TableCell>
                      {spread ? (
                        (spread.ask - spread.bid).toFixed(2)
                      ) : (
                        <Chip label="N/A" size="small" />
                      )}
                    </TableCell>
                    <TableCell>
                      {spread && spread.lastTradePrice ? (
                        <Box display="flex" alignItems="center">
                          <Typography>
                            {spread.lastTradePrice.toFixed(2)}
                          </Typography>
                          {spread.lastTradeTime && (
                            <Typography variant="caption" color="text.secondary" ml={1}>
                              {new Date(spread.lastTradeTime).toLocaleTimeString()}
                            </Typography>
                          )}
                        </Box>
                      ) : (
                        <Chip label="No trades" size="small" />
                      )}
                    </TableCell>
                    <TableCell>
                      <Button 
                        variant="outlined" 
                        size="small"
                        onClick={(e) => {
                          e.stopPropagation();
                          navigateToStock(stock.symbol);
                        }}
                      >
                        View Details
                      </Button>
                    </TableCell>
                  </TableRow>
                );
              })}
            </TableBody>
          </Table>
        </TableContainer>
      )}

      {/* Add Stock Dialog */}
      <Dialog open={openAddDialog} onClose={() => setOpenAddDialog(false)}>
        <DialogTitle>Add New Stock</DialogTitle>
        <DialogContent>
          <Box component="form" sx={{ mt: 2 }}>
            <Grid container spacing={2}>
              <Grid item xs={12}>
                <TextField
                  label="Symbol"
                  fullWidth
                  required
                  value={newStock.symbol}
                  onChange={(e) => setNewStock(prev => ({ ...prev, symbol: e.target.value.toUpperCase() }))}
                  inputProps={{ maxLength: 5 }}
                  helperText="Max 5 characters, uppercase"
                />
              </Grid>
              <Grid item xs={12}>
                <TextField
                  label="Name"
                  fullWidth
                  required
                  value={newStock.name}
                  onChange={(e) => setNewStock(prev => ({ ...prev, name: e.target.value }))}
                />
              </Grid>
              <Grid item xs={12}>
                <TextField
                  label="Description"
                  fullWidth
                  multiline
                  rows={3}
                  value={newStock.description}
                  onChange={(e) => setNewStock(prev => ({ ...prev, description: e.target.value }))}
                />
              </Grid>
              {addError && (
                <Grid item xs={12}>
                  <Typography color="error">{addError}</Typography>
                </Grid>
              )}
            </Grid>
          </Box>
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setOpenAddDialog(false)}>Cancel</Button>
          <Button 
            onClick={handleAddStock} 
            variant="contained"
            disabled={addingStock}
          >
            {addingStock ? 'Adding...' : 'Add Stock'}
          </Button>
        </DialogActions>
      </Dialog>
    </Container>
  );
};

export default StocksListPage;