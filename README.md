# Fast Limit Order Book
This repo simulates an order book using a red black tree to hold pointers to limit structs, and two hash maps - one for all orders and one for all limits. I found some information about how an order book may be designed [here](https://web.archive.org/web/20110219163448/http://howtohft.wordpress.com/2011/02/15/how-to-build-a-fast-limit-order-book/). The order book is coded in C++ and compiled into a library. Incorporating [pybind11](https://github.com/pybind/pybind11) into the project allows us to expose the C++ order book class to python. Pybind11 allows us to either create a .so or wheel file. Either of which can then be imported from the python runtime.

### To get started 
1. clone the repository and `cd order_book`
2. bring in the pybind11 dependency: `git clone git@github.com:pybind/pybind11.git`
3. start a python virual enviornment: `pipenv shell`
4. install python dependencies: `pipenv install`
5. create a python wheel and install it: `make wheel`
6. start a python repl and: `import book`

### Time, Space, and Data Structures
* let n and k represent the number of limits and the number of orders per limit, respectivly.
* std::set will be of size O(n)
* std::unordered_map will be of size O(n)
* best case O(1) time to add an order, worst case O(log(n))
* worst case O(1) time to process matches, worst canse O(n*k). Such a large variance exists because one bid order could potentially match with n ask orders, or visa versa. 


### Other Notes
* May need to remove the book's wheel from the pipfile if it has not been created yet
* If you are trying to compile and build the wheel but not noticing changes being reflected, you might need to either remove the physical dependency which can be found with `make check` or bump the code version inside the setup.py file before running `make wheel`
* a buyer  matches with a seller when the incoming buy  price is higher than a sell offer
* a seller matches with a buyer  when the incoming sell price is lower  than a buy offer 

#### Todo 
* what to do with extra money 
* cancel order 
* python stuff
* intermediary class between the book and python


#### Future ideas
* only run the bots during market hours
* small websocket show how much memory/cpu the process is consuming 
* load balancer for incoming requests to distributed order_book processes 
* [stock exchange design](https://www.youtube.com/watch?v=XuKs2kWH0mQ&ab_channel=System-Design)
