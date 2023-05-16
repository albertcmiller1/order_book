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

    OrderBook ob;
    
    int order_id = 123456;
    bool buy_sell = true;
    int shares = 1;
    float limit = 23.42;
    int entry_time = 983485;
    int event_time = 983485;

    ob.add_order(
        order_id,
        buy_sell,
        shares,
        limit,
        entry_time,
        event_time
    );

    order_id = 654321;
    buy_sell = false;
    shares = 2;
    limit = 23.44;
    entry_time = 983485;
    event_time = 983485;

    ob.add_order(
        order_id,
        buy_sell,
        shares,
        limit,
        entry_time,
        event_time
    );

    // ob.order_map.erase(654321);

    // Limit *root = ob.insert_new_limit(nullptr, 12);
    // cout << root->limit_price << endl;
    // cout << root->left << endl;
    // cout << root->right << endl;
    
    // cout << "\n";

    // root = ob.insert_new_limit(root, 14);
    // cout << root->limit_price << endl;
    // cout << root->left << endl;
    // cout << root->right << endl;
    // cout << root->right->limit_price << endl;

    cout << ob << endl;
}