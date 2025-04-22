#!/bin/bash

# Colors for better visibility
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${GREEN}Starting Order Book Server...${NC}"

# Check for pipenv
if ! command -v pipenv &> /dev/null; then
    echo "Pipenv is not installed. Please install it first with 'pip install pipenv'"
    exit 1
fi

# Setup virtual environment and install dependencies if needed
if [ ! -d "$(pipenv --venv 2>/dev/null)" ]; then
    echo -e "${BLUE}Setting up Python virtual environment...${NC}"
    pipenv install
fi

# Check if uvicorn is installed
if ! pipenv run pip list | grep -q uvicorn; then
    echo -e "${BLUE}Installing uvicorn...${NC}"
    pipenv install uvicorn
fi

# Check if FastAPI is installed
if ! pipenv run pip list | grep -q fastapi; then
    echo -e "${BLUE}Installing FastAPI...${NC}"
    pipenv install fastapi
fi

# Check node and npm
if ! command -v npm &> /dev/null; then
    echo "npm is not installed. Please install Node.js and npm first."
    exit 1
fi

# Check if frontend dependencies are installed
if [ ! -d "frontend/node_modules" ]; then
    echo -e "${BLUE}Installing frontend dependencies...${NC}"
    (cd frontend && npm install)
fi

# Start the backend in the background
echo -e "${GREEN}Starting backend server...${NC}"
pipenv run python -c "import uvicorn; uvicorn.run('src.main.py.main:app', host='127.0.0.1', port=8000, reload=True)" &
BACKEND_PID=$!

# Wait a bit for the backend to start
sleep 2

# Start the frontend in the foreground
echo -e "${GREEN}Starting frontend server...${NC}"
(cd frontend && npm start)

# Cleanup function to kill the background process when the script exits
cleanup() {
    echo -e "${GREEN}Shutting down servers...${NC}"
    kill $BACKEND_PID 2>/dev/null
    echo -e "${GREEN}Done!${NC}"
}

# Register the cleanup function to be called on exit
trap cleanup EXIT