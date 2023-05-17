#include "main.hpp"
using namespace std; 
  
int main()
{
    /* 
        binary tree holds a tree of limit prices 
        each node holds a LL of orders at that price 

        when an order comes in 
            > if price is in limt_map 
                > go into it's LL and append 
                > update tree node's volume 
            > else
                > create new tree node 
                > create new LL on tree node 
    */

    OrderBook book;
    book = create_fake_orders(book);
    cout << book << endl;
}

OrderBook create_fake_orders(OrderBook book){
    // book.order_map.erase(654321);
    book.add_order(
        123456,         // order_id
        true,           // buy_sell
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

    book.add_order(
        345252,         // order_id
        true,           // buy_sell
        1,              // shares
        23.43,          // limit
        983485,         // entry_time
        983485          // event_time
    );

    return book;
}