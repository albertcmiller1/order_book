# Fast Limit Order Book

## Overview 
### Book
This repo simulates an order book using a doubly linked list of unique limit prices, a doubly linked list of orders at each limit price node, and two hash maps - one of all limits and one of all orders. I found some information about how an order book may be designed [here](https://web.archive.org/web/20110219163448/http://howtohft.wordpress.com/2011/02/15/how-to-build-a-fast-limit-order-book/). This repo is intended to be used in conjunction with the [paper_trader](https://github.com/albertcmiller1/paper_trader) repository. 

### Connecting to the book
The orderbook hosts an api, made with [crow](https://crowcpp.org/master/), which will allow a user to post an order, cancel an order, and check the current status of their order. This api will also allow an admin to list or delist a new stock at an 'IPO' price.

### Streaming data from the book 
The orderbook hosts a websocket, also made with [crow](https://crowcpp.org/master/). The socket will broadcast to each connected user any time a match is created. This socket will also continuously boradcast the prices of the 10 highest buy limits and the 10 lowest sell limits surrounding the spread. This data will be used to create an 'order book vizualizer' made in the [paper_trader](https://github.com/albertcmiller1/paper_trader) repository. 

### Trading bots
To simulate market activity, 3 threads will continuously post buy and sell orders at random prices for each stock listed. This will stimulate the book enough for users to stream live prices, and enable a user to activly post buy and sell orders with paper stocks using the [paper_trader](https://github.com/albertcmiller1/paper_trader) repository. 

### Price data API
Every 60 seconds the book will post the current price of each stock traded into a dynamodb table. This information will be accessable to users via an API hosted with AWS API Gateway + lambda created with the [paper_trader](https://github.com/albertcmiller1/paper_trader) repository.

### Infrastructure 
The book, trading bot threads, api, and websocket will be apart of the same process which will be running on an AWS EC2 instance. 

## Todo 
* tests 
* print 100 as 100.00
* find a way to make all incoming orders limit price values of dollars and cents format 
* api (post order, cancel order, check current price)
* validate inputs 
* broadcast to socket
* limit should hold the total volume of num shares contained in its order dll
* logic to retract from queue 
* botoCpp to post transaction (updated price) to dynamodb
* remove total_volume and num_limit_nodes ?

## Future ideas
* how to build a driver/buffer for much faster performance? 
* other data structure ideas for the order book? increase speed? 
* frontend to view the spread. broadcast the needed info using socket [example](https://www.youtube.com/watch?v=hgOXY-r3xJM&ab_channel=ChadThackray)
* only run the bots during market hours
* maybe using an AWS SQS would be better for holding buy and sell orders? then we could have a more distrubuted matching service aka more scalable 
* would be cool to host a small websocket (python) on the EC2 to show how much memory/cpu the server has used 
* [stock exchange design](https://www.youtube.com/watch?v=XuKs2kWH0mQ&ab_channel=System-Design)


23.41 buy 
23.42 buy 
23.43 buy 
23.44 buy 
SPREAD
23.46 sell
23.47 sell
23.48 sell
23.49 sell

a buyer  matches with a  seller when the incoming buy  price is greater than a sell offer
a seller matches with a  buyer  when the incoming sell price is less    than a buy offer 


Right now, if I post a buy order for 23.50, ill get a crossed the spread message
but if I post a buy order for 23.49, it will match up both buyer and seller at 23.49, when it probably should match up the 23.49 buyer with the 23.46 seller. 


> 23.41/buy/1 23.42/buy/1 23.43/buy/1 23.44/buy/1     
> then a sell order for 2 shares at 23.44


existing: 
23.41:  1018663624/buy/3/23.410000 
23.42:  1162029806/buy/3/23.420000 
23.43:  2016074196/buy/3/23.430000 
23.44:  2048834942/buy/3/23.440001 
23.45:  801515218/sell/3/23.450001 
23.46:  2072714208/sell/3/23.459999 1775455869/sell/3/23.459999 
23.47:  1235562093/sell/3/23.469999 
incoming: 
buy 10 @ 23.46


create a map <std::string ticker, OrderBook book> to hold all a unique book for each unique ticker 
create api endpoint to IPO a stock, submit an order, cancel an order, check properties of the book
socket should broadcast all trades, and the current state of the spread, and the limit dll






head

buy 
buy 
buy 
buy 
SPREAD
98.4:       -1666021676/sell/10/98.400000 
98.93:      -1802087656/sell/10/98.930000 
99.29:      -1015061220/sell/10/99.290000 -1060229120/sell/10/99.290000 
99.73:      1850612224/sell/10/99.730000 
99.95:      1807663346/sell/10/99.950000 
100:        -1470214806/sell/10/100.000000 
100.29:     1809307880/sell/10/100.290000 
100.39:     2092683790/sell/10/100.390000 
100.41:     1249714992/sell/10/100.410000 
100.44:     1048110644/sell/10/100.440000 
100.68:     1090232368/sell/10/100.680000 
100.72:     1204932772/sell/10/100.720000 


tail 

_________incoming order________
| shares: 10
| order_type: buy
| limit: 98.61
| order_id: 1095041655