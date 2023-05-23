#include "main.hpp"
#include <stdio.h>
#include <string.h>
using namespace std; 

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

    /*
    if (arg_map.find("order_id") == arg_map.end()) {
        // this price is not in the limit map-- update limit_map. 
    } else {
        // this price is already in the limit_map. 
    }
    */

    // cout << arg_map["order_id"] << endl;
    // cout << arg_map["limit"] << endl;
    // cout << arg_map["shares"] << endl;
    // cout << arg_map["buy_sell"] << endl;
    // cout << arg_map["event_time"] << endl;
    // cout << arg_map["entry_time"] << endl;

    return arg_map;
}

int main(int argc, char** argv){
    OrderBook book;
    book = create_fake_orders(book);
    cout << book << endl;
    return 0;
}

OrderBook create_fake_orders(OrderBook book){
    
    /*
    // only do this if arg_map is fully populated 
    std::unordered_map<string, char*> arg_map = parse_args(argc, argv);
    book.add_order(
        atoi(arg_map["order_id"]),                  // int order_id
        !strcmp("buy", arg_map["buy_sell"]),        // bool buy_sell
        atoi(arg_map["shares"]),                    // int shares
        std::stof(arg_map["limit"]),                // float limit
        atoi(arg_map["entry_time"]),                // int entry_time
        atoi(arg_map["event_time"])                 // int event_time
    );
    */

    book.add_order(
        000001,         // order_id
        1,              // buy_sell
        1,              // shares
        23.44,          // limit
        983485,         // entry_time
        983485          // event_time
    );

    book.add_order(
        000002,         // order_id
        true,           // buy_sell
        1,              // shares
        23.44,          // limit
        983485,         // entry_time
        983485          // event_time
    );

    book.add_order(
        000003,         // order_id
        true,           // buy_sell
        1,              // shares
        23.44,          // limit
        983485,         // entry_time
        983485          // event_time
    );

    book.add_order(
        345555,         // order_id
        true,           // buy_sell
        1,              // shares
        23.45,          // limit
        983485,         // entry_time
        983485          // event_time
    );    
    
    book.add_order(
        355257,         // order_id
        true,           // buy_sell
        1,              // shares
        23.45,          // limit
        983485,         // entry_time
        983485          // event_time
    ); 

    return book;
}