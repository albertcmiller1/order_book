#include "order_book.hpp"
#include <iostream> 
#include <unordered_set>
#include <mutex>
#include <iostream>
#include <thread>
#include <unistd.h>  
#include <cstdlib>
#include <chrono>
// #include <unistd.h>



// looks like we have an issue here bc the main function uses the same parameters. 
// std::unordered_map<string, char*> parse_args(int argc, char** argv);

int main(int argc, char** argv);
OrderBook create_fake_orders(OrderBook book);