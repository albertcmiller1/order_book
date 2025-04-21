import React, { useEffect, useState } from 'react';
import { 
  LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, 
  ResponsiveContainer, Legend 
} from 'recharts';
import { Paper, Typography, CircularProgress, Box, Select, MenuItem, FormControl, InputLabel } from '@mui/material';
import { ChartData } from '../types';
import { getChartData } from '../services/api';

interface StockChartProps {
  symbol: string;
}

const StockChart: React.FC<StockChartProps> = ({ symbol }) => {
  const [chartData, setChartData] = useState<ChartData[]>([]);
  const [timeframe, setTimeframe] = useState<string>('1d');
  const [loading, setLoading] = useState<boolean>(true);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    const fetchChartData = async () => {
      setLoading(true);
      setError(null);
      
      try {
        const data = await getChartData(symbol, timeframe);
        setChartData(data);
      } catch (err) {
        console.error('Error fetching chart data:', err);
        setError('Failed to load chart data');
      } finally {
        setLoading(false);
      }
    };

    fetchChartData();
    
    // Refresh chart data every minute
    const intervalId = setInterval(fetchChartData, 60000);
    
    return () => clearInterval(intervalId);
  }, [symbol, timeframe]);

  const handleTimeframeChange = (event: any) => {
    setTimeframe(event.target.value);
  };

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

  return (
    <Paper sx={{ p: 2 }}>
      <Box display="flex" justifyContent="space-between" alignItems="center" mb={2}>
        <Typography variant="h6">{symbol} Price Chart</Typography>
        <FormControl size="small" sx={{ minWidth: 120 }}>
          <InputLabel>Timeframe</InputLabel>
          <Select
            value={timeframe}
            label="Timeframe"
            onChange={handleTimeframeChange}
          >
            <MenuItem value="1h">1 Hour</MenuItem>
            <MenuItem value="1d">1 Day</MenuItem>
            <MenuItem value="1w">1 Week</MenuItem>
            <MenuItem value="1m">1 Month</MenuItem>
          </Select>
        </FormControl>
      </Box>
      <ResponsiveContainer width="100%" height={300}>
        <LineChart
          data={chartData}
          margin={{ top: 5, right: 30, left: 20, bottom: 5 }}
        >
          <CartesianGrid strokeDasharray="3 3" />
          <XAxis dataKey="time" />
          <YAxis domain={['auto', 'auto']} />
          <Tooltip />
          <Legend />
          <Line
            type="monotone"
            dataKey="price"
            stroke="#8884d8"
            activeDot={{ r: 8 }}
          />
          {chartData[0]?.volume !== undefined && (
            <Line type="monotone" dataKey="volume" stroke="#82ca9d" />
          )}
        </LineChart>
      </ResponsiveContainer>
    </Paper>
  );
};

export default StockChart;