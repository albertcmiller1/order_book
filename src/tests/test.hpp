#include "../book/order_book.hpp"
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <random>

class Testing {
public: 
    unsigned int microseconds = 10000;
    bool logging = true;

    std::string build_order(OrderBook *book, std::string order_type, int shares, double limit);
    bool doubles_are_same(double a, double b);
    void clean_up(OrderBook *book);
    bool test_1();
    bool test_2();
    bool test_3();
    bool test_4();
    bool test_5();
    bool test_6();
    bool test_7();
    bool test_8();
    bool test_9();
    bool test_10();
    bool test_11();
    bool test_12();
    bool test_13();
    bool test_14();
    bool test_15();
    bool test_16();
    bool test_17();
    bool test_18();
    bool test_19();
    bool test_20();
};