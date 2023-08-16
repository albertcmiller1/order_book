# Fast Limit Order Book

## Overview 
### Book
This repo simulates an order book using a doubly linked list of unique limit prices, a doubly linked list of orders at each limit price node, and two hash maps - one of all limits and one of all orders. I found some information about how an order book may be designed [here](https://web.archive.org/web/20110219163448/http://howtohft.wordpress.com/2011/02/15/how-to-build-a-fast-limit-order-book/). This repo is intended to be used in conjunction with the [paper_trader](https://github.com/albertcmiller1/paper_trader) repository. 

### Connecting to the book
The orderbook hosts an api, made with [crow](https://github.com/CrowCpp/Crow), which will allow a user to post an order, cancel an order, and check the current status of their order. This api will also allow an admin to list or delist a new stock at an 'IPO' price.

### Streaming data from the book 
The orderbook hosts a websocket, also made with [crow](https://github.com/CrowCpp/Crow). The socket will continuously boradcast the prices of the 10 highest buy limits and the 10 lowest sell limits surrounding the spread. This data will be used to create an 'order book vizualizer' made in the [paper_trader](https://github.com/albertcmiller1/paper_trader) repository. 

### Trading bots
To simulate market activity, 3 threads will continuously post buy and sell orders at random prices for each stock listed. This will stimulate the book enough for users to stream live prices, and enable a user to activly post buy and sell orders with paper stocks using the [paper_trader](https://github.com/albertcmiller1/paper_trader) repository. 

### Price data API
Every 60 seconds the book will post the current price of each stock traded into a dynamodb table. This information will be accessable to users via an API hosted with AWS API Gateway + lambda created with the [paper_trader](https://github.com/albertcmiller1/paper_trader) repository.

### Infrastructure 
The book, trading bot threads, api, and websocket will be apart of the same process which will be running on an AWS EC2 instance. 

## To get started 
1. clone the repository 
2. `mkdir build && cd build`
3. `cmake ..`
4. `make` 

## Future ideas
* convert limit DLL to tree
* how to build a driver/buffer for much faster performance
* frontend to view the spread. broadcast the needed info using socket [example](https://www.youtube.com/watch?v=hgOXY-r3xJM&ab_channel=ChadThackray)
* only run the bots during market hours
* maybe using an AWS SQS would be better for holding buy and sell orders? then we could have a more distrubuted matching service aka more scalable 
* would be cool to host a small websocket (python) on the EC2 to show how much memory/cpu the server has used 
* [stock exchange design](https://www.youtube.com/watch?v=XuKs2kWH0mQ&ab_channel=System-Design)

## Todo 
* more tests 
* speed up update_limit_spread_new()
* print 100 as 100.00
* validate api request inputs 
* counts (num orders, num limits, volumes, etc)
* aws sdk to post transaction (updated price) to dynamodb
* remove total_volume and num_limit_nodes?
* python program to test multiple users firing off orders 

## notes 
* a buyer  matches with a  seller when the incoming buy  price is greater than a sell offer
* a seller matches with a  buyer  when the incoming sell price is less    than a buy offer 

TODO: 
14,080 orders/sec wo logging 

to run tests: 
* `g++ -g -Wall -std=c++20 main.cpp order_book.cpp`
* `valgrind --tool=memcheck --leak-check=yes ./a.out`


would be cool to have some kinda load balancer if too many order requests are coming in at the same time. if the book is getting so much traffic that's its slowing down, maybe have multiple processes of the same stock, and somehow ensure the price is always the same. not sure if this is possible or not 