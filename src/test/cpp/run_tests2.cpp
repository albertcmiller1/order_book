#include "run_tests2.hpp"

int main(){
    Testing tests = Testing();

    tests.ob_can_add_orders();
    tests.ob_can_match_one_order_equal_shares();
    tests.ob_can_match_one_order_non_equal_shares_asks();
    tests.ob_can_match_one_order_non_equal_shares_bids();
    tests.ob_can_traverse_bid_limit_deque();
    tests.ob_can_traverse_ask_limit_deque();
    tests.ob_can_traverse_bid_limits();
    tests.ob_can_traverse_ask_limits();

    return 0;
}