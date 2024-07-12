# Fast Limit Order Book
This repo simulates an order book using a red black tree to hold pointers to limit structures, and two hash maps - one for all orders and one for all limits. I found some information about how an order book may be designed [here](https://web.archive.org/web/20110219163448/http://howtohft.wordpress.com/2011/02/15/how-to-build-a-fast-limit-order-book/). The order book is coded in C++ and compiled into a library. [pybind11](https://github.com/pybind/pybind11)  allows us to either create a shared object or wheel file which can then be installed, imported, and called from python.

### Getting started 
1. clone the repository and `cd order_book`
2. bring in the pybind11 dependency: `git clone git@github.com:pybind/pybind11.git`
3. start a python virual enviornment: `pipenv shell`
4. install python dependencies: `pipenv install`
5. create a python wheel and install it: `make wheel`
6. start a python repl and: `import book`

### Space, Time, and Data Structures
* let N represent the number of limits 
* let K represent the average number of orders per limit
* let J represent the number of matches required to process orders that crossed the spread
* red black tree will be of size O(N), and take O(log(N)) to insert, remove, and seach
* unordered hash map will be of size O(N), and take amortized O(1) to insert, delete, and update
* to add an Order to the book: best case O(1) time, and worst case O(log(n))
* to process J matches: best case O(J) time, worst case of O(J x (K+log(N))) 

### Using the Book 
* instantiate the book: `ob = book.OrderBook()`
* add a bid:  `ob.add_order(book.OrderType.bid, "albert", 6, 100.23)`
* add an ask: `ob.add_order(book.OrderType.ask, "albert", 6, 100.23)`
* get the number of orders: `ob.num_orders(book.OrderType.bid)`
* get the number of limits: `ob.num_limits(book.OrderType.bid)`
* get the lowest ask limit price: `ob.prominent_limit(book.OrderType.bid)`
* get the highest bid limit price: `ob.prominent_limit(book.OrderType.ask)`
* return 10 most prominent limits: `ob.get_limits(book.OrderType.bid, 10)`

### Other Notes
* May need to remove the book's wheel from the pipfile if it has not been created yet
* If you are trying to compile and build the wheel but not noticing changes being reflected, you might need to either remove the physical dependency which can be found with `make check` or bump the code version inside the setup.py file before running `make wheel`
* a bid matches with one or many asks when the incoming bid price is higher than the most prominent (lowest) asks
* an ask matches with one or many bids when the incoming ask price is lower than the most prominent (highest) bids

#### Todo 
* fix log floating point doubles... make money class?
* what to do with extra money 
* python api
* intermediary class between the book and python
* seeing some ob.get_limits(book.OrderType.ask, 10)[0] != ob.prominent_limit(book.OrderType.bid)
* > could be formatting? 
* much slower since introducing Money

#### Future ideas
* only run the bots during market hours
* small websocket show how much memory/cpu the process is consuming 
* load balancer for incoming requests to distributed order_book processes 
* [stock exchange design](https://www.youtube.com/watch?v=XuKs2kWH0mQ&ab_channel=System-Design)
