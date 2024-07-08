#include <iostream>
#include <queue>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

enum class OrderType { 
    bid, 
    ask 
};

struct Order {
    const std::string order_id;
    const std::string user_id;
    int shares;
    std::string entry_time;
    // uint64_t entry_time;
    Order(std::string id, std::string uid, int s, std::string t): 
        order_id(std::move(id)), 
        user_id(std::move(uid)), 
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

struct CompareLimitHeap {
    bool operator()(const std::shared_ptr<Limit>& p1, const std::shared_ptr<Limit>& p2) const {
        if (p1->type != p2->type){ throw; }
        if (p1->type == OrderType::ask){
            return p1->limit_price > p2->limit_price; 
        }
        if (p1->type == OrderType::bid){
            return p1->limit_price < p2->limit_price;
        }
        throw std::invalid_argument("Comparing two limits of non uniform type");;
    }
};

class OrderBook2 {
public: 
    void print_bid_limits();
    void print_ask_limits();

    std::string add_order(
        const OrderType &order_type, 
        const std::string &user_id, 
        const int &shares, 
        const double &limit_price
    );

    std::vector<double> get_top_n_bids();
    std::vector<double> get_top_n_asks();

private: 
    // why cant i &type
    template<typename Map, typename Heap>
    std::shared_ptr<Limit> get_or_create_limit(Map &limit_map, Heap &heap, OrderType type, double limit_price);

    /*
    if i have a function like 
    &Limit dostuff();
    Limit& dostuff(); ??
    does that return an address or a reference? 
    */

    std::priority_queue<
        std::shared_ptr<Limit>, 
        std::vector<std::shared_ptr<Limit>>, 
        CompareLimitHeap
    > maxBidLimitHeap;

    std::priority_queue<
        std::shared_ptr<Limit>, 
        std::vector<std::shared_ptr<Limit>>, 
        CompareLimitHeap
    > minAskLimitHeap;

    std::unordered_map<double, std::shared_ptr<Limit>> bid_limit_map;
    std::unordered_map<double, std::shared_ptr<Order>> bid_order_map;
    std::vector<std::shared_ptr<Limit>> top_bid_limits;
    
    std::unordered_map<double, std::shared_ptr<Limit>> ask_limit_map;
    std::unordered_map<double, std::shared_ptr<Order>> ask_order_map;
    std::vector<std::shared_ptr<Limit>> top_ask_limits; // stack or queue?
};