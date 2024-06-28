# Fast Limit Order Book
This repo simulates an order book using a doubly linked list of unique limit prices, a doubly linked list of orders at each limit price node, and two hash maps - one of all limits and one of all orders. I found some information about how an order book may be designed [here](https://web.archive.org/web/20110219163448/http://howtohft.wordpress.com/2011/02/15/how-to-build-a-fast-limit-order-book/). The book itself is coded in C++, which is then compiled with [Pybind11](https://github.com/pybind/pybind11) into a python libary. 


### To get started 
1. clone the repository
2. bring in the pybind11 dependency: `git clone git@github.com:pybind/pybind11.git`
3. start a python virual enviornment: `pipenv shell`
4. install python dependencies: `pipenv install`
5. compile the C++ library and create pybind module: `make compile`
6. create a python wheel and install it: `make wheel`
7. run the entry function: `make run`



### Notes on the book 
* a buyer  matches with a seller when the incoming buy  price is higher than a sell offer
* a seller matches with a buyer  when the incoming sell price is lower  than a buy offer 
* buy  offers will be higher than the spread, and at the front of the limit DLL
* sell offers will be lower  than the spread, and at the back  of the lmit DLL

#### Todo 
* Change everything to bid and ask 
* why isnt most recent trade price always in the middle of the spread
* why am i still able to access garbage after i delete a pointer
* more tests 
* destructors
* speed up update_limit_spread_new()
* counts (num orders, num limits, volumes, etc)
* remove total_volume and num_limit_nodes?
* add user_id to the Orders / Matches 

#### Future ideas
* convert limit DLL to tree
* its O(N) to insert a new limit that does not exist yet. Any way to make this faster using binary search?
* how to build a driver/buffer for much faster performance
* only run the bots during market hours
* would be cool to host a small websocket (python) on the EC2 to show how much memory/cpu the server has used 
* [stock exchange design](https://www.youtube.com/watch?v=XuKs2kWH0mQ&ab_channel=System-Design)
* load balancer for incoming requests to distributed order_book processes 
* frontend to view the spread. broadcast the needed info using socket. [example](https://www.youtube.com/watch?v=hgOXY-r3xJM&ab_channel=ChadThackray)
