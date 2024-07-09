#include "run_tests2.hpp"

int main(){
    Testing tests = Testing();

    tests.ob_can_add_orders();
    tests.ob_can_match_one_order_equal_shares();
    tests.ob_can_match_one_order_non_equal_shares_asks();
    tests.ob_can_match_one_order_non_equal_shares_bids();

    return 0;
}