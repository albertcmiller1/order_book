#include "../../main/cpp/order_book2.hpp"
#include <algorithm>
#include <vector>

class Testing {
public: 
    bool logging;
    void ob_can_add_orders();
    void ob_can_match_one_order_equal_shares();

    void ob_can_match_one_order_non_equal_shares_bids();
    void ob_can_match_one_order_non_equal_shares_asks();
    
    void ob_can_traverse_bid_limit_deque();
    void ob_can_traverse_ask_limit_deque();

    void ob_can_traverse_bid_limits();
    void ob_can_traverse_ask_limits();
};