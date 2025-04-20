#include "../../main/cpp/order_book.hpp"
#include <algorithm>
#include <vector>
#include <cassert>

class Testing {
private: 
    template<typename V>
    void compare_vectors(V &v1, V &v2);

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

    void ob_can_check_if_order_in_queue();
    
    void ob_can_cancel_an_order_bid();
    void ob_can_cancel_an_order_ask();

    void test_money();
};