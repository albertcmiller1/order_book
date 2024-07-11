#include "test.hpp"

void Testing::ob_can_add_orders(){
    OrderBook ob = OrderBook();
    ob.add_order(OrderType::ask, "user1", 3, 104);
    ob.add_order(OrderType::ask, "user1", 3, 103);
    ob.add_order(OrderType::ask, "user1", 3, 102);
    ob.add_order(OrderType::ask, "user1", 3, 101);

    ob.add_order(OrderType::bid, "user1", 3, 99); 
    ob.add_order(OrderType::bid, "user1", 3, 98);
    ob.add_order(OrderType::bid, "user1", 3, 98);

    assert(ob.num_limits(OrderType::bid) == 2);
    assert(ob.num_orders(OrderType::bid) == 3);
    assert(ob.prominent_limit(OrderType::bid) == 99);

    assert(ob.num_limits(OrderType::ask) == 4);
    assert(ob.num_orders(OrderType::ask) == 4);
    assert(ob.prominent_limit(OrderType::ask) == 101);

    std::vector<double> expected_bid_limits {99, 98};
    std::vector<double> bid_limits = ob.get_limits(OrderType::bid, 2);
    assert(expected_bid_limits.size() == bid_limits.size());
    assert(std::equal(expected_bid_limits.begin(), expected_bid_limits.end(), bid_limits.begin()));

    std::vector<double> expected_ask_limits {101, 102, 103, 104};
    std::vector<double> ask_limits = ob.get_limits(OrderType::ask);
    assert(expected_ask_limits.size() == ask_limits.size());
    assert(std::equal(expected_ask_limits.begin(), expected_ask_limits.end(), ask_limits.begin()));

    std::cout << "[ PASSED ] ob_can_add_orders" << std::endl;
}

void Testing::ob_can_match_one_order_equal_shares(){
    OrderBook ob = OrderBook();
    ob.add_order(OrderType::ask, "user1", 3, 100);
    ob.add_order(OrderType::bid, "user1", 3, 100);

    std::vector<Match> matches = ob.process();
    assert(matches.size() == 1);

    assert(ob.num_limits(OrderType::bid) == 0);
    assert(ob.num_orders(OrderType::bid) == 0);
    assert(ob.prominent_limit(OrderType::bid) == -1);

    assert(ob.num_limits(OrderType::ask) == 0);
    assert(ob.num_orders(OrderType::ask) == 0);
    assert(ob.prominent_limit(OrderType::ask) == -1);

    assert(ob.get_limits(OrderType::bid).empty());
    assert(ob.get_limits(OrderType::ask).empty());

    std::cout << "[ PASSED ] ob_can_match_one_order_equal_shares" << std::endl;
}

void Testing::ob_can_match_one_order_non_equal_shares_asks(){
    OrderBook ob = OrderBook();
    ob.add_order(OrderType::ask, "user1", 10, 100);
    ob.add_order(OrderType::bid, "user1", 5, 100);

    std::vector<Match> matches = ob.process();
    assert(matches.size() == 1);

    assert(ob.num_limits(OrderType::bid) == 0);
    assert(ob.num_orders(OrderType::bid) == 0);
    assert(ob.prominent_limit(OrderType::bid) == -1);

    assert(ob.num_limits(OrderType::ask) == 1);
    assert(ob.num_orders(OrderType::ask) == 1);
    assert(ob.prominent_limit(OrderType::ask) == 100);

    assert(ob.get_limits(OrderType::bid).empty());
    assert(!ob.get_limits(OrderType::ask).empty());

    std::cout << "[ PASSED ] ob_can_match_one_order_non_equal_shares_asks" << std::endl;
}

void Testing::ob_can_match_one_order_non_equal_shares_bids(){
    OrderBook ob = OrderBook();
    ob.add_order(OrderType::ask, "user1", 5, 100);
    ob.add_order(OrderType::bid, "user1", 10, 100);

    std::vector<Match> matches = ob.process();
    assert(matches.size() == 1);

    assert(ob.num_limits(OrderType::bid) == 1);
    assert(ob.num_orders(OrderType::bid) == 1);
    assert(ob.prominent_limit(OrderType::bid) == 100);

    assert(ob.num_limits(OrderType::ask) == 0);
    assert(ob.num_orders(OrderType::ask) == 0);
    assert(ob.prominent_limit(OrderType::ask) == -1);

    assert(!ob.get_limits(OrderType::bid).empty());
    assert(ob.get_limits(OrderType::ask).empty());

    std::cout << "[ PASSED ] ob_can_match_one_order_non_equal_shares_bids" << std::endl;
}

void Testing::ob_can_traverse_bid_limit_deque(){
    OrderBook ob = OrderBook();
    ob.add_order(OrderType::ask, "user1", 30, 100);
    ob.add_order(OrderType::bid, "user1", 10, 100);
    ob.add_order(OrderType::bid, "user1", 10, 100);
    ob.add_order(OrderType::bid, "user1", 10, 100);

    std::vector<Match> matches = ob.process();
    assert(matches.size() == 3);

    assert(ob.num_limits(OrderType::bid) == 0);
    assert(ob.num_orders(OrderType::bid) == 0);
    assert(ob.prominent_limit(OrderType::bid) == -1);

    assert(ob.num_limits(OrderType::ask) == 0);
    assert(ob.num_orders(OrderType::ask) == 0);
    assert(ob.prominent_limit(OrderType::ask) == -1);

    assert(ob.get_limits(OrderType::bid).empty());
    assert(ob.get_limits(OrderType::ask).empty());

    std::cout << "[ PASSED ] ob_can_traverse_bid_limit_deque" << std::endl;
}

void Testing::ob_can_traverse_ask_limit_deque(){
    OrderBook ob = OrderBook();
    ob.add_order(OrderType::ask, "user1", 10, 100);
    ob.add_order(OrderType::ask, "user1", 10, 100);
    ob.add_order(OrderType::ask, "user1", 10, 100);
    ob.add_order(OrderType::bid, "user1", 30, 100);

    std::vector<Match> matches = ob.process();
    assert(matches.size() == 3);

    assert(ob.num_limits(OrderType::bid) == 0);
    assert(ob.num_orders(OrderType::bid) == 0);
    assert(ob.prominent_limit(OrderType::bid) == -1);

    assert(ob.num_limits(OrderType::ask) == 0);
    assert(ob.num_orders(OrderType::ask) == 0);
    assert(ob.prominent_limit(OrderType::ask) == -1);

    assert(ob.get_limits(OrderType::bid).empty());
    assert(ob.get_limits(OrderType::ask).empty());

    std::cout << "[ PASSED ] ob_can_traverse_ask_limit_deque" << std::endl;
}

void Testing::ob_can_traverse_bid_limits(){
    OrderBook ob = OrderBook();
    ob.add_order(OrderType::ask, "user1", 20, 99);
    ob.add_order(OrderType::bid, "user1", 10, 101);
    ob.add_order(OrderType::bid, "user1", 10, 100);

    std::vector<Match> matches = ob.process();
    assert(matches.size() == 2);

    assert(ob.num_limits(OrderType::bid) == 0);
    assert(ob.num_orders(OrderType::bid) == 0);
    assert(ob.prominent_limit(OrderType::bid) == -1);

    assert(ob.num_limits(OrderType::ask) == 0);
    assert(ob.num_orders(OrderType::ask) == 0);
    assert(ob.prominent_limit(OrderType::ask) == -1);

    assert(ob.get_limits(OrderType::bid).empty());
    assert(ob.get_limits(OrderType::ask).empty());

    std::cout << "[ PASSED ] ob_can_traverse_bid_limits" << std::endl;
}

void Testing::ob_can_traverse_ask_limits(){
    OrderBook ob = OrderBook();
    ob.add_order(OrderType::ask, "user1", 10, 101);
    ob.add_order(OrderType::ask, "user1", 10, 100);
    ob.add_order(OrderType::bid, "user1", 20, 103);

    std::vector<Match> matches = ob.process();
    assert(matches.size() == 2);

    assert(ob.num_limits(OrderType::bid) == 0);
    assert(ob.num_orders(OrderType::bid) == 0);
    assert(ob.prominent_limit(OrderType::bid) == -1);

    assert(ob.num_limits(OrderType::ask) == 0);
    assert(ob.num_orders(OrderType::ask) == 0);
    assert(ob.prominent_limit(OrderType::ask) == -1);

    assert(ob.get_limits(OrderType::bid).empty());
    assert(ob.get_limits(OrderType::ask).empty());

    std::cout << "[ PASSED ] ob_can_traverse_ask_limits" << std::endl;
}

void Testing::ob_can_cancel_an_order(){
    OrderBook ob = OrderBook();
    std::string order_id = ob.add_order(OrderType::bid, "user1", 10, 101);
    
    ob.cancel_order(order_id);


    assert(ob.num_limits(OrderType::bid) == 0);
    assert(ob.num_orders(OrderType::bid) == 0);
    assert(ob.prominent_limit(OrderType::bid) == -1);

    std::cout << "[ PASSED ] ob_can_cancel_an_order" << std::endl;
}