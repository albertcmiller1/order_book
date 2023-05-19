#include "main.hpp"
#include <stdio.h>
#include <string.h>
using namespace std; 

/* 
    binary tree holds a tree of limit prices 
    each node holds a LL of orders at that price 

    when an order comes in 
        > if price in limt_map 
            > go into it's LL and append 
            > update tree node's volume 
        > else
            > create new tree node 
            > create new LL on tree node 
*/

std::unordered_map<string, char*> parse_args(int argc, char** argv){
    std::unordered_map<string, char*> arg_map = {};
    for (int i = 1; i < argc; ++i){
        if (!strcmp("--order_id", argv[i])){
            arg_map["order_id"] = argv[i+1];
        } else if (!strcmp("--limit", argv[i])){
            arg_map["limit"] = argv[i+1];
        } else if (!strcmp("--shares", argv[i])){
            arg_map["shares"] = argv[i+1];
        } else if (!strcmp("--buy_sell", argv[i])){
            // validate string == buy or sell 
            arg_map["buy_sell"] =  argv[i+1];
        } else if (!strcmp("--event_time", argv[i])){
            arg_map["event_time"] = argv[i+1];
        } else if (!strcmp("--entry_time", argv[i])){
            arg_map["entry_time"] = argv[i+1];
        }
    }

    // cout << arg_map["order_id"] << endl;
    // cout << arg_map["limit"] << endl;
    // cout << arg_map["shares"] << endl;
    // cout << arg_map["buy_sell"] << endl;
    // cout << arg_map["event_time"] << endl;
    // cout << arg_map["entry_time"] << endl;

    return arg_map;
}

int main(int argc, char** argv){
    
    std::unordered_map<string, char*> arg_map = parse_args(argc, argv);
    OrderBook book;

    // only do this if arg_map is fully populated 
    book.add_order(
        atoi(arg_map["order_id"]),                  // int order_id
        !strcmp("buy", arg_map["buy_sell"]),        // bool buy_sell
        atoi(arg_map["shares"]),                    // int shares
        std::stof(arg_map["limit"]),                // float limit
        atoi(arg_map["entry_time"]),                // int entry_time
        atoi(arg_map["event_time"])                 // int event_time
    );

    book = create_fake_orders(book);
    cout << book << endl;
    return 0;
}

OrderBook create_fake_orders(OrderBook book){
    // book.order_map.erase(654321);

    book.add_order(
        123456,         // order_id
        1,              // buy_sell
        1,              // shares
        23.42,          // limit
        983485,         // entry_time
        983485          // event_time
    );

    book.add_order(
        654321,         // order_id
        true,           // buy_sell
        1,              // shares
        23.44,          // limit
        983485,         // entry_time
        983485          // event_time
    );

    // book.add_order(
    //     345252,         // order_id
    //     true,           // buy_sell
    //     1,              // shares
    //     23.43,          // limit
    //     983485,         // entry_time
    //     983485          // event_time
    // );

    // book.add_order(
    //     345257,         // order_id
    //     true,           // buy_sell
    //     1,              // shares
    //     23.43,          // limit
    //     983485,         // entry_time
    //     983485          // event_time
    // );    
    
    // book.add_order(
    //     355257,         // order_id
    //     true,           // buy_sell
    //     1,              // shares
    //     23.4,          // limit
    //     983485,         // entry_time
    //     983485          // event_time
    // ); 

    return book;
}