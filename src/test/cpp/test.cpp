#include "test.hpp"

template<typename V>
void Testing::compare_vectors(V &v1, V &v2){
    if (v1.size() != v2.size()){
        throw std::invalid_argument("Unequal array size.");
    }
    for (int i=0; i<v1.size(); ++i){
        assert(v1[i]==v2[i]);
    }
}

void Testing::ob_can_add_orders(){
    OrderBook ob = OrderBook();
    ob.add_order(OrderType::ask, "user1", 3, Money(104));
    ob.add_order(OrderType::ask, "user1", 3, Money(103));
    ob.add_order(OrderType::ask, "user1", 3, Money(102));
    ob.add_order(OrderType::ask, "user1", 3, Money(101));

    ob.add_order(OrderType::bid, "user1", 3, Money(99)); 
    ob.add_order(OrderType::bid, "user1", 3, Money(98));
    ob.add_order(OrderType::bid, "user1", 3, Money(98));

    assert(ob.num_limits(OrderType::bid) == 2);
    assert(ob.num_orders(OrderType::bid) == 3);
    assert(ob.prominent_limit(OrderType::bid).getDollars() == 99);

    assert(ob.num_limits(OrderType::ask) == 4);
    assert(ob.num_orders(OrderType::ask) == 4);
    assert(ob.prominent_limit(OrderType::ask).getDollars() == 101);

    std::vector<Money> expected_bids {Money(99, 0), Money(98, 0)};
    std::vector<Money> bid_limits = ob.get_limits(OrderType::bid, 2);
    this->compare_vectors(expected_bids, bid_limits);


    std::vector<Money> expected_asks {Money(101, 0), Money(102, 0), Money(103, 0), Money(104, 0)};
    std::vector<Money> ask_limits = ob.get_limits(OrderType::ask);
    this->compare_vectors(expected_asks, ask_limits);
    

    std::cout << "[ PASSED ] ob_can_add_orders" << std::endl;
}

void Testing::ob_can_match_one_order_equal_shares(){
    OrderBook ob = OrderBook();
    ob.add_order(OrderType::ask, "user1", 3, Money(100, 0));
    ob.add_order(OrderType::bid, "user1", 3, Money(100, 0));

    std::vector<Match> matches = ob.process();
    assert(matches.size() == 1);

    assert(ob.num_limits(OrderType::bid) == 0);
    assert(ob.num_orders(OrderType::bid) == 0);
    assert(ob.prominent_limit(OrderType::bid).getDollars() == -1);

    assert(ob.num_limits(OrderType::ask) == 0);
    assert(ob.num_orders(OrderType::ask) == 0);
    assert(ob.prominent_limit(OrderType::ask).getDollars() == -1);

    assert(ob.get_limits(OrderType::bid).empty());
    assert(ob.get_limits(OrderType::ask).empty());

    std::cout << "[ PASSED ] ob_can_match_one_order_equal_shares" << std::endl;
}

void Testing::ob_can_match_one_order_non_equal_shares_asks(){
    OrderBook ob = OrderBook();
    ob.add_order(OrderType::ask, "user1", 10, Money(100, 0));
    ob.add_order(OrderType::bid, "user1", 5, Money(100, 0));

    std::vector<Match> matches = ob.process();
    assert(matches.size() == 1);

    assert(ob.num_limits(OrderType::bid) == 0);
    assert(ob.num_orders(OrderType::bid) == 0);
    assert(ob.prominent_limit(OrderType::bid).getDollars() == -1);

    assert(ob.num_limits(OrderType::ask) == 1);
    assert(ob.num_orders(OrderType::ask) == 1);
    assert(ob.prominent_limit(OrderType::ask).getDollars() == 100);

    assert(ob.get_limits(OrderType::bid).empty());
    assert(!ob.get_limits(OrderType::ask).empty());

    std::cout << "[ PASSED ] ob_can_match_one_order_non_equal_shares_asks" << std::endl;
}

void Testing::ob_can_match_one_order_non_equal_shares_bids(){
    OrderBook ob = OrderBook();
    ob.add_order(OrderType::ask, "user1", 5, Money(100, 0));
    ob.add_order(OrderType::bid, "user1", 10, Money(100, 0));

    std::vector<Match> matches = ob.process();
    assert(matches.size() == 1);

    assert(ob.num_limits(OrderType::bid) == 1);
    assert(ob.num_orders(OrderType::bid) == 1);
    assert(ob.prominent_limit(OrderType::bid).getDollars() == 100);

    assert(ob.num_limits(OrderType::ask) == 0);
    assert(ob.num_orders(OrderType::ask) == 0);
    assert(ob.prominent_limit(OrderType::ask).getDollars() == -1);

    assert(!ob.get_limits(OrderType::bid).empty());
    assert(ob.get_limits(OrderType::ask).empty());

    std::cout << "[ PASSED ] ob_can_match_one_order_non_equal_shares_bids" << std::endl;
}

void Testing::ob_can_traverse_bid_limit_deque(){
    OrderBook ob = OrderBook();
    ob.add_order(OrderType::ask, "user1", 30, Money(100, 0));
    ob.add_order(OrderType::bid, "user1", 10, Money(100, 0));
    ob.add_order(OrderType::bid, "user1", 10, Money(100, 0));
    ob.add_order(OrderType::bid, "user1", 10, Money(100, 0));

    std::vector<Match> matches = ob.process();
    assert(matches.size() == 3);

    assert(ob.num_limits(OrderType::bid) == 0);
    assert(ob.num_orders(OrderType::bid) == 0);
    assert(ob.prominent_limit(OrderType::bid).getDollars() == -1);

    assert(ob.num_limits(OrderType::ask) == 0);
    assert(ob.num_orders(OrderType::ask) == 0);
    assert(ob.prominent_limit(OrderType::ask).getDollars() == -1);

    assert(ob.get_limits(OrderType::bid).empty());
    assert(ob.get_limits(OrderType::ask).empty());

    std::cout << "[ PASSED ] ob_can_traverse_bid_limit_deque" << std::endl;
}

void Testing::ob_can_traverse_ask_limit_deque(){
    OrderBook ob = OrderBook();
    ob.add_order(OrderType::ask, "user1", 10, Money(100, 0));
    ob.add_order(OrderType::ask, "user1", 10, Money(100, 0));
    ob.add_order(OrderType::ask, "user1", 10, Money(100, 0));
    ob.add_order(OrderType::bid, "user1", 30, Money(100, 0));

    std::vector<Match> matches = ob.process();
    assert(matches.size() == 3);

    assert(ob.num_limits(OrderType::bid) == 0);
    assert(ob.num_orders(OrderType::bid) == 0);
    assert(ob.prominent_limit(OrderType::bid).getDollars() == -1);

    assert(ob.num_limits(OrderType::ask) == 0);
    assert(ob.num_orders(OrderType::ask) == 0);
    assert(ob.prominent_limit(OrderType::ask).getDollars() == -1);

    assert(ob.get_limits(OrderType::bid).empty());
    assert(ob.get_limits(OrderType::ask).empty());

    std::cout << "[ PASSED ] ob_can_traverse_ask_limit_deque" << std::endl;
}

void Testing::ob_can_traverse_bid_limits(){
    OrderBook ob = OrderBook();
    ob.add_order(OrderType::ask, "user1", 20, Money(99, 0));
    ob.add_order(OrderType::bid, "user1", 10, Money(101, 0));
    ob.add_order(OrderType::bid, "user1", 10, Money(100, 0));

    std::vector<Match> matches = ob.process();
    assert(matches.size() == 2);

    assert(ob.num_limits(OrderType::bid) == 0);
    assert(ob.num_orders(OrderType::bid) == 0);
    assert(ob.prominent_limit(OrderType::bid).getDollars() == -1);

    assert(ob.num_limits(OrderType::ask) == 0);
    assert(ob.num_orders(OrderType::ask) == 0);
    assert(ob.prominent_limit(OrderType::ask).getDollars() == -1);

    assert(ob.get_limits(OrderType::bid).empty());
    assert(ob.get_limits(OrderType::ask).empty());

    std::cout << "[ PASSED ] ob_can_traverse_bid_limits" << std::endl;
}

void Testing::ob_can_traverse_ask_limits(){
    OrderBook ob = OrderBook();
    ob.add_order(OrderType::ask, "user1", 10, Money(101, 0));
    ob.add_order(OrderType::ask, "user1", 10, Money(100, 0));
    ob.add_order(OrderType::bid, "user1", 20, Money(103, 0));

    std::vector<Match> matches = ob.process();
    assert(matches.size() == 2);

    assert(ob.num_limits(OrderType::bid) == 0);
    assert(ob.num_orders(OrderType::bid) == 0);
    assert(ob.prominent_limit(OrderType::bid).getDollars() == -1);

    assert(ob.num_limits(OrderType::ask) == 0);
    assert(ob.num_orders(OrderType::ask) == 0);
    assert(ob.prominent_limit(OrderType::ask).getDollars() == -1);

    assert(ob.get_limits(OrderType::bid).empty());
    assert(ob.get_limits(OrderType::ask).empty());

    std::cout << "[ PASSED ] ob_can_traverse_ask_limits" << std::endl;
}

void Testing::ob_can_cancel_an_order_bid(){
    OrderBook ob = OrderBook();
    std::string order_id = ob.add_order(OrderType::bid, "user1", 10, Money(101, 0));
    
    ob.cancel_order(order_id);
    assert(ob.num_limits(OrderType::bid) == 0);
    assert(ob.num_orders(OrderType::bid) == 0);
    assert(ob.prominent_limit(OrderType::bid).getDollars() == -1);

    std::cout << "[ PASSED ] ob_can_cancel_an_order_bid" << std::endl;
}

void Testing::ob_can_cancel_an_order_ask(){
    OrderBook ob = OrderBook();
    std::string order_id = ob.add_order(OrderType::ask, "user1", 10, Money(101, 0));
    
    ob.cancel_order(order_id);
    assert(ob.num_limits(OrderType::ask) == 0);
    assert(ob.num_orders(OrderType::ask) == 0);
    assert(ob.prominent_limit(OrderType::ask).getDollars() == -1);

    std::cout << "[ PASSED ] ob_can_cancel_an_order_ask" << std::endl;
}

void Testing::test_money(){
    Money m1 = Money(100, 101);
    assert(m1.getCents() == 1);
    assert(m1.getDollars() == 101);

    Money m2 = Money(10);
    Money m3 = Money(20);
    assert(m3>m2);
    assert(m3+m2==Money(30));
    assert(m3+m2==Money(30));


    std::cout << "[ PASSED ] test_money" << std::endl;

}