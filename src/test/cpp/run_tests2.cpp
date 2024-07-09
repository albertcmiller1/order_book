#include "run_tests2.hpp"

int main(){
    Testing tests = Testing();

    tests.ob_can_add_orders();
    tests.ob_can_match_one_order_equal_shares();

    return 0;
}