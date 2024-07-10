#include <iostream>
#include <queue>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <random>
#include <chrono>
#include <set>
#include <algorithm>

enum class OrderType { 
    bid, 
    ask 
};

struct Order {
    const std::string order_id;
    const std::string user_id;
    int shares;
    std::string entry_time;
    Order(std::string o_id, std::string u_id, int s, std::string t): 
        order_id(std::move(o_id)), 
        user_id(std::move(u_id)), 
        shares(std::move(s)),
        entry_time(std::move(t)) {}
};

struct Limit {
    const double limit_price;
    const OrderType type;    
    std::deque<std::shared_ptr<Order>> orders;
    Limit(double lp, OrderType t): 
        limit_price(std::move(lp)), 
        type(std::move(t)) {}
};

struct Match {
    std::string match_id;
    std::string bid_order_id;
    std::string ask_order_id;
    int shares;
    double limit_price;
    Match(): 
        match_id(std::move("Unknown")),
        bid_order_id(std::move("Unknown")),
        ask_order_id(std::move("Unknown")),
        shares(std::move(-1)),
        limit_price(std::move(-1)) {}
    Match(std::string m_id, std::string b_id, std::string a_id, int s, double lp): 
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
    double prominent_limit(OrderType type);
    std::vector<double> get_limits(OrderType type, int n=10);
    std::string add_order(
        const OrderType &order_type, 
        const std::string &user_id, 
        const int &shares, 
        const double &limit_price
    );
    std::vector<Match> process();
private: 
    // why cant i &type on get_or_create_limit
    template<typename Map, typename Set>
    std::shared_ptr<Limit> get_or_create_limit(Map &limit_map, Set &set, OrderType type, double limit_price);
    std::string generate_order_id();
    std::string get_cur_time();
    
    
    std::shared_ptr<Limit> prominent_limit_ptr(OrderType type); //TODO test this 
    Match create_match(std::shared_ptr<Limit> &ask_limit, std::shared_ptr<Limit> &bid_limit);
    bool can_match();

    std::set<std::shared_ptr<Limit>, CompareLimit> bid_limits; // O(log(N)) insertions, deletions, searches
    std::unordered_map<double, std::shared_ptr<Limit>> bid_limit_map;
    std::unordered_map<std::string, std::shared_ptr<Order>> bid_order_map;
    
    std::set<std::shared_ptr<Limit>, CompareLimit> ask_limits; 
    std::unordered_map<double, std::shared_ptr<Limit>> ask_limit_map;
    std::unordered_map<std::string, std::shared_ptr<Order>> ask_order_map;

    friend class Testing;
};