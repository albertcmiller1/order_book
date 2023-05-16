#include "main.hpp"
using namespace std; 
  
int main()
{
    OrderBook ob;
    
    ob.add_ask(23.40, 1);
    ob.add_ask(23.29, 1);
    ob.add_ask(23.32, 1);
    ob.add_ask(23.34, 1);

    ob.add_bid(23.26, 1);
    ob.add_bid(23.27, 1);
    ob.add_bid(23.28, 1);
    ob.add_bid(23.25, 1);

    cout << ob << endl;
}