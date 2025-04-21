import React, { useEffect } from 'react';
import { BrowserRouter as Router, Routes, Route, Link } from 'react-router-dom';
import { 
  CssBaseline, Box, AppBar, Toolbar, Typography, 
  Container, Button, Divider
} from '@mui/material';
import { ThemeProvider, createTheme } from '@mui/material/styles';
import StocksListPage from './pages/StocksListPage';
import StockDetailPage from './pages/StockDetailPage';
import OrdersPage from './pages/OrdersPage';
import websocketService from './services/websocket';

// Create a theme instance
const theme = createTheme({
  palette: {
    mode: 'light',
    primary: {
      main: '#1976d2',
    },
    secondary: {
      main: '#dc004e',
    },
  },
});

function App() {
  // Connect to WebSocket when app starts
  useEffect(() => {
    websocketService.connect().catch(err => {
      console.error('Failed to connect to WebSocket:', err);
    });

    // Clean up WebSocket connection when app unmounts
    return () => {
      websocketService.disconnect();
    };
  }, []);

  return (
    <ThemeProvider theme={theme}>
      <Router>
        <CssBaseline />
        <Box sx={{ display: 'flex', flexDirection: 'column', minHeight: '100vh' }}>
          <AppBar position="static">
            <Toolbar>
              <Typography variant="h6" component={Link} to="/" sx={{ 
                flexGrow: 1, 
                textDecoration: 'none', 
                color: 'inherit' 
              }}>
                Order Book Exchange
              </Typography>
              <Button color="inherit" component={Link} to="/">
                Stocks
              </Button>
              <Button color="inherit" component={Link} to="/orders">
                My Orders
              </Button>
            </Toolbar>
          </AppBar>
          <Box component="main" sx={{ flexGrow: 1, bgcolor: 'background.default', py: 3 }}>
            <Routes>
              <Route path="/" element={<StocksListPage />} />
              <Route path="/stocks/:symbol" element={<StockDetailPage />} />
              <Route path="/orders" element={<OrdersPage />} />
            </Routes>
          </Box>
          <Box component="footer" sx={{ p: 2, mt: 'auto', bgcolor: 'background.paper' }}>
            <Divider sx={{ mb: 2 }} />
            <Container maxWidth="lg">
              <Typography variant="body2" color="text.secondary" align="center">
                © {new Date().getFullYear()} Order Book Exchange - Trading Platform
              </Typography>
            </Container>
          </Box>
        </Box>
      </Router>
    </ThemeProvider>
  );
}

export default App;