#include "main.hpp"
#include <stdio.h>
#include <string.h>
using namespace std; 


void create_fake_orders_1(OrderBook &book){
    
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
        1,                  // shares
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
        1,                  // shares
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
        1,                  // shares
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
        1,                  // shares
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
        1,                  // shares
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
        1,                  // shares
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
        1,                  // shares
        23.41,              // limit
        curr_time,          // entry_time
        curr_time           // event_time
    ); 


        

    // triggers the match 
    order_id = rand();
    usleep(microseconds);
    curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    book.add_order(
        1111111111,         // order_id
        "buy",              // order_type
        1,                  // shares
        23.47,              // limit
        curr_time,          // entry_time
        curr_time           // event_time
    );

    // should be 3 sell orders
    // should be 3 buy orders 
    // values at each data point should match what is expected

}

void create_fake_orders_3(OrderBook &book){
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


    // trigger 
    order_id = rand();
    usleep(microseconds);
    curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    book.add_order(
        1111111111,           // order_id
        "buy",              // order_type
        10,                  // shares
        23.46,              // limit
        curr_time,          // entry_time
        curr_time           // event_time
    ); 
}

void create_fake_orders_4(OrderBook &book){

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


    // trigger 
    order_id = rand();
    usleep(microseconds);
    curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    book.add_order(
        1111111111,           // order_id
        "buy",              // order_type
        7,                  // shares
        23.46,              // limit
        curr_time,          // entry_time
        curr_time           // event_time
    ); 
}

