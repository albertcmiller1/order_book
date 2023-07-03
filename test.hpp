// #include "order_book.hpp"
#include <stdio.h>
#include <string.h>
 

class Testing {
public: 
    int create_order(OrderBook *book, std::string order_type, int shares, float limit);
    bool test_1(OrderBook *book);
    bool test_2(OrderBook *book);
    bool test_3(OrderBook *book);
};