#include <iostream>
#include <queue>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <random>
#include <chrono>
#include <set>
#include "money.hpp"

enum class OrderType { 
    bid, 
    ask 
};

struct Order {
    const std::string order_id;
    const std::string user_id;
    Money limit_price;
    int shares;
    std::string entry_time;
    Order(std::string o_id, std::string u_id, Money lp, int s, std::string t): 
        order_id(std::move(o_id)), 
        user_id(std::move(u_id)), 
        limit_price(std::move(lp)), 
        shares(std::move(s)),
        entry_time(std::move(t)) {}
};

struct Limit {
    const Money limit_price;
    const OrderType type;    
    std::deque<std::shared_ptr<Order>> orders;
    Limit(Money lp, OrderType t): 
        limit_price(std::move(lp)), 
        type(std::move(t)) {}
};

struct Match {
    std::string match_id;
    std::string bid_order_id;
    std::string ask_order_id;
    int shares;
    Money limit_price;
    Match(): 
        match_id(std::move("Unknown")),
        bid_order_id(std::move("Unknown")),
        ask_order_id(std::move("Unknown")),
        shares(std::move(-1)),
        limit_price(std::move(-1)) {}
    Match(std::string m_id, std::string b_id, std::string a_id, int s, Money lp): 
        match_id(std::move(m_id)),
        bid_order_id(std::move(b_id)),
        ask_order_id(std::move(a_id)),
        shares(std::move(s)),
        limit_price(std::move(lp)) {}
};

struct CompareLimit {
    bool operator()(const std::shared_ptr<Limit>& p1, const std::shared_ptr<Limit>& p2) const {
        if (p1->type != p2->type){ 
            throw std::invalid_argument("Cannot compare two limits of non-uniform type.");
        }
        if (p1->type == OrderType::ask){
            return p1->limit_price < p2->limit_price; 
        }
        return p1->limit_price > p2->limit_price;
    }
};

class OrderBook {
public: 
    int num_orders(OrderType type);
    int num_limits(OrderType type);
    Money prominent_limit(OrderType type);
    std::vector<Money> get_limits(OrderType type, int n=10);
    std::vector<Match> process();
    std::string add_order(
        const OrderType &order_type, 
        const std::string &user_id, 
        const int &shares, 
        const Money limit_price
    );
    void cancel_order(std::string order_id);
    bool order_in_queue(std::string &order_id);

private: 
    template<typename Map, typename Set>
    std::shared_ptr<Limit> get_or_create_limit(Map &limit_map, Set &set, OrderType type, Money limit_price);
    
    template<typename OrderMap, typename LimitMap, typename Set>
    void remove_order(OrderMap &order_map, LimitMap &limit_map, Set &limits, std::string &order_id);

    bool can_match();
    std::string get_cur_time();
    std::string generate_order_id();
    Match create_match(std::shared_ptr<Limit> ask_limit, std::shared_ptr<Limit> bid_limit);
    
    std::set<std::shared_ptr<Limit>, CompareLimit> bid_limits; 
    std::unordered_map<Money, std::shared_ptr<Limit>, MoneyHasher> bid_limit_map;
    std::unordered_map<std::string, std::shared_ptr<Order>> bid_order_map;
    
    std::set<std::shared_ptr<Limit>, CompareLimit> ask_limits; 
    std::unordered_map<Money, std::shared_ptr<Limit>, MoneyHasher> ask_limit_map;
    std::unordered_map<std::string, std::shared_ptr<Order>> ask_order_map;

    friend class Testing;
};

