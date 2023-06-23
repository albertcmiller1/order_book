// #include "main.hpp"
#include "test.cpp"
#include <stdio.h>
#include <string.h>
using namespace std; 


void create_fake_orders_(OrderBook &book){
    // using namespace std::chrono;
    // std::cout << ms << " milliseconds since the Epoch\n";
    
    srand((unsigned) time(NULL));
    int order_id = rand();
    unsigned int microseconds {10000};

    uint64_t curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    order_id = rand();
    usleep(microseconds);
    curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    book.add_order(
        order_id,           // order_id
        "sell",             // order_type
        3,                  // shares
        23.47,              // limit
        curr_time,          // entry_time
        curr_time           // event_time
    );

    order_id = rand();
    usleep(microseconds);
    curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    book.add_order(
        order_id,           // order_id
        "sell",             // order_type
        3,                  // shares
        23.46,              // limit
        curr_time,          // entry_time
        curr_time           // event_time
    ); 

    order_id = rand();
    usleep(microseconds);
    curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    book.add_order(
        order_id,           // order_id
        "sell",             // order_type
        3,                  // shares
        23.46,              // limit
        curr_time,          // entry_time
        curr_time           // event_time
    ); 

    order_id = rand();
    usleep(microseconds);
    curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    book.add_order(
        order_id,           // order_id
        "sell",             // order_type
        3,                  // shares
        23.45,              // limit
        curr_time,          // entry_time
        curr_time           // event_time
    ); 

    // SPREAD

    order_id = rand();
    usleep(microseconds);
    curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    book.add_order(
        order_id,           // order_id
        "buy",              // order_type
        3,                  // shares
        23.44,              // limit
        curr_time,          // entry_time
        curr_time           // event_time
    );

    order_id = rand();
    usleep(microseconds);
    curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    book.add_order(
        order_id,           // order_id
        "buy",              // order_type
        3,                  // shares
        23.43,              // limit
        curr_time,          // entry_time
        curr_time           // event_time
    );

    order_id = rand();
    usleep(microseconds);
    curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    book.add_order(
        order_id,           // order_id
        "buy",              // order_type
        3,                  // shares
        23.42,              // limit
        curr_time,          // entry_time
        curr_time           // event_time
    ); 


    order_id = rand();
    usleep(microseconds);
    curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    book.add_order(
        order_id,           // order_id
        "buy",              // order_type
        3,                  // shares
        23.41,              // limit
        curr_time,          // entry_time
        curr_time           // event_time
    ); 
}


void create_fake_orders_2(OrderBook &book){
    
    srand((unsigned) time(NULL));
    int order_id = rand();
    uint64_t curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    order_id = rand();
    unsigned int microseconds {10000};
    usleep(microseconds);
    curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    book.add_order(
        1111111111,         // order_id
        "sell",              // order_type
        100,                  // shares
        23.41,              // limit
        curr_time,          // entry_time
        curr_time           // event_time
    );
}

int main(int argc, char** argv){
    OrderBook book;
    create_fake_orders_(book);
    std::cout << book << std::endl;
    std::cout << "\n\n<-------------------------------------------------------->\n";
    create_fake_orders_2(book);
    std::cout << book << std::endl;
    return 0;

    // what happens if we have a ll

    // ptr1 -> ptr2 -> ptr3 -> NULL
    // we make ptr to ptr2
    // delete ptr2

    // does ptr still point to existing node? 
}