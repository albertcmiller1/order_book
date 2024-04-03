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
Every 60 seconds the book will use an [HTTP header](https://github.com/elnormous/HTTPRequest) to post the current price of each stock to an AWS lambda which will prost to an AWS dynamodb table. This AWS lambda and price history data will be built and accessable in the [paper_trader](https://github.com/albertcmiller1/paper_trader) repository.

### Infrastructure 
The book, trading bot threads, api, and websocket will be apart of the same process which will be running on an AWS EC2 instance. 

## To get started 
1. clone the repository 
2. `mkdir build && cd build`
3. `cmake ..`
4. `make` 

## Notes 
book 
* a buyer  matches with a seller when the incoming buy  price is higher than a sell offer
* a seller matches with a buyer  when the incoming sell price is lower  than a buy offer 
* buy  offers will be higher than the spread, and at the front of the limit DLL
* sell offers will be lower  than the spread, and at the back  of the lmit DLL

run tests: 
* `g++ -g -Wall -std=c++20 main.cpp order_book.cpp -o tests`
* `valgrind --tool=memcheck --leak-check=yes ./tests`

socket endpoints
* `wscat -c ws://0.0.0.0:5001/spread`
* `wscat -c ws://0.0.0.0:5001/matches`
* `wscat -c ws://0.0.0.0:5001/price`

api endpoints
* `curl http://0.0.0.0:5001/curr_price`
* `curl http://0.0.0.0:5001/place_order`
* `curl http://0.0.0.0:5001/order_status`
* `curl http://0.0.0.0:5001/cancel_order`
* `curl http://0.0.0.0:5001/get_spread`

start and connect to server 
* `nohup ./socket &> socket.log &`
* `wscat -c 3.95.214.132:5001/ws`
* `curl http://3.95.214.132:5001/curr_price`

speed 
* 14,080 orders/sec w/o logging (average after 1M orders)

questions 
* why isnt most recent trade price always in the middle of the spread
* why am i still able to access garbage after i delete a pointer

## Todo 
* more tests 
* destructors
* speed up update_limit_spread_new()
* counts (num orders, num limits, volumes, etc)
* aws sdk or http library for posting matches
* remove total_volume and num_limit_nodes?
* python program to test multiple users firing off orders 
* either configure Dynamo for CPP or use [MySQL](https://dev.mysql.com/doc/mysql-getting-started/en/)

## Future ideas
* convert limit DLL to tree
* how to build a driver/buffer for much faster performance
* frontend to view the spread. broadcast the needed info using socket. [example](https://www.youtube.com/watch?v=hgOXY-r3xJM&ab_channel=ChadThackray)
* only run the bots during market hours
* maybe using an AWS SQS would be better for holding buy and sell orders? then we could have a more distrubuted matching service aka more scalable 
* would be cool to host a small websocket (python) on the EC2 to show how much memory/cpu the server has used 
* [stock exchange design](https://www.youtube.com/watch?v=XuKs2kWH0mQ&ab_channel=System-Design)
* load balancer for incoming requests to distributed order_book processes 

https://docs.aws.amazon.com/sdk-for-cpp/v1/developer-guide/getting-started.html
https://github.com/awsdocs/aws-doc-sdk-examples/blob/main/cpp/example_code/s3/list_buckets.cpp
https://github.com/awsdocs/aws-doc-sdk-examples/blob/6a3ab3b2c742ebe96c97b23f715a7a5b6a69548c/cpp/example_code/s3/include/awsdoc/s3/s3_examples.h#L4



Change everything to bid and ask 
add user_id to the Orders / Matches 