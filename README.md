# Fast Limit Order Book

## Overview 
### Book
This repo simulates an order book using a doubly linked list and two hash maps. Each entry in the limit_map contains a unique limit price. Each of these entries has a pointer to a linked list containing orders. I found some information about how an order book may be designed [here](https://web.archive.org/web/20110219163448/http://howtohft.wordpress.com/2011/02/15/how-to-build-a-fast-limit-order-book/).

### Connecting to the book
The orderbook hosts a websocket, made with [crow](https://crowcpp.org/master/), which will allow a user to post an order, cancel an order, and check the current status of their order. This socket will also allow an admin to list or delist a new stock at an "IPO" price. Finally, the orderbook will broadcast to the socket any time an exchange occurs.

### Trading bots
To simulate market activity, 26 threads will continuously post buy and sell orders at random prices. This will stimulate the book enough for users to stream live prices, and enable a user to activly post buy and sell orders with paper stocks using the [paper_trader](https://github.com/albertcmiller1/paper_trader) repository. 

### Price data API
Every 60 seconds the book will post the current price of each stock traded into a dynamodb table. This information will be accessable to users via an API hosted with AWS API Gateway + lambda.

### Infrastructure 
The book, trading bot threads, and connection websocket will be apart of the same process. It will be running on a docker container hosted by AWS ECS. 

## Todo 
* implement trees + DLL
* implement spread check + erase after match
* logic to retract from queue 
* who get's priority in queue? time dependent? what about partial ability to complete transactions due to volume differences 
* boto to post transaction (updated price) to dynamodb
* [stock exchange design](https://www.youtube.com/watch?v=XuKs2kWH0mQ&ab_channel=System-Design)

## Future ideas
* not sure we can have both an API and a WS hosted as the same time. Instead, just use an ws to both brodcast info and accept incoming messages? 
* how to build a driver for much faster performance? 
* other data structure ideas for the order book?
* frontend to view the spread. broadcast the needed info using socket [example](https://www.youtube.com/watch?v=hgOXY-r3xJM&ab_channel=ChadThackray)
* convert all lambdas to python
* only run the bots during market hours
* maybe using an AWS SQS would be better for holding buy and sell orders? then we could have a more distrubuted matching service aka more scalable 

## Schemas 

Order Doubly Linked List
```
struct Order {
    int order_id;
    bool buy_sell;
    int shares;
    float limit;
    string ticker;
    int entry_time;
    <!-- int event_time; -->
    string status; -> in-progress, failed, filled, partially-filled
    Order* next {nullptr};
    Order* prev {nullptr};
    Limit* parent_limit {nullptr};
};
```
Limit Binary Search Tree
```
struct Limit {
    float limit_price;
    int size;
    int total_volume;
    Limit *parent {nullptr};
    Limit *left {nullptr};
    Limit *right {nullptr};
    Order *head_order {nullptr};
    Order *tail_order {nullptr};
};
```
Matched Orders 
```
struct Match {
    string match_id;
    int buying_order_id;
    int selling_order_id;
    int sale_quantity;
    float sale_price;
}
```