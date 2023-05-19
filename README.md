# Fast Limit Order Book

## Overview 
### Book
This repo simulates an order book using a binary tree, doubly linked list, and two hash maps.  Each node of the binary tree holds a unique limit price, and a pointer to a doubly linked list (sorted by time) containing bids and asks. I found some information about how an order book may be designed [here](https://web.archive.org/web/20110219163448/http://howtohft.wordpress.com/2011/02/15/how-to-build-a-fast-limit-order-book/).

### Connecting to the book
The orderbook will host an API which will allow a user to post an order to buy or sell, cancel an order, and check the current price of the stock. note -> maybe should not include price check, instead leave it to the price data api.  

### Trading bots
To simulate market activity, 26 threads will continuously post buy and sell orders at random prices. This will stimulate the book enough for users to stream live prices, and enable a user to activly post buy and sell orders with paper stocks using the [paper_trader](https://github.com/albertcmiller1/paper_trader) repository. 

### Sockets
The orderbook will connect to a websocket hosted on AWS. The orderbook will broadcast to the socket any time an exchange occurs. The socket will be hosted with API Gateway, 4 lambda functions, and a dynamodb table.

### Price data API
Each time an echange occurs, the book will post the transaction into a dynamodb table. Additionaly, Every 60 seconds the book will post the current price of each stock traded into a dynamodb table. This data will be accessable to users via an API hosted with AWS API Gateway + lambda.

### Infrastructure 
The book, trading bot threads, and connection API will be apart of the same process. It will be running on a docker container hosted by AWS ECS. 

## Todo 
* implement trees + DLL
* implement spread check + erase after match
* logic to retract from queue 
* who get's priority in queue? time dependent? what about partial ability to complete transactions due to volume differences 
* boto to post transaction (updated price) to dynamodb

## Future ideas
* how to build a driver for much faster performance? 
* other data structure ideas for the order book?
* frontend to view the spread 
* convert all lambdas to python
