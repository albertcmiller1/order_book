import React from 'react';
import { Container, Typography, Box } from '@mui/material';
import OrdersList from '../components/OrdersList';

const OrdersPage: React.FC = () => {
  // Mock user ID (in a real app this would come from auth)
  const userId = "user123";

  return (
    <Container maxWidth="lg" sx={{ mt: 4, mb: 4 }}>
      <Box mb={3}>
        <Typography variant="h4">
          Your Orders
        </Typography>
      </Box>
      
      <OrdersList userId={userId} />
    </Container>
  );
};

export default OrdersPage;