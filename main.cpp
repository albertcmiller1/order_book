#include "main.hpp"
using namespace std; 
  
int main()
{
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

    // cout << ob.order_map[123456].limit << endl;
    // cout << ob.order_map[654321].limit << endl;

    for (auto i = ob.order_map.begin(); i != ob.order_map.end(); i++)
        cout << i->first << "  " << i->second.id_number << endl;

    ob.order_map.erase(654321);

    for (auto i = ob.order_map.begin(); i != ob.order_map.end(); i++)
        cout << i->first << "  " << i->second.id_number << endl;

}