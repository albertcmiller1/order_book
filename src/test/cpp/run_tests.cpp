#include "run_tests.hpp"

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
    tests.ob_can_cancel_an_order_bid();
    tests.ob_can_cancel_an_order_ask();
    tests.test_money();

    return 0;
}