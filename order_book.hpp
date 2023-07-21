#include <iostream>
#include <iomanip>
#include <unistd.h>  
#include <cstdlib>
#include <chrono>
#include <vector> 
#include <stdexcept>

struct Match {
    int match_id;
    int buying_order_id;
    int selling_order_id;
    int sale_quantity;
    double sale_price;
};

struct Order {
    int order_id;
    std::string order_type;
    // int customer_id;
    int shares;
    double limit;
    unsigned long long entry_time;
    unsigned long long event_time;
    Order* next {nullptr};
    Order* prev {nullptr};
};

struct Limit {
    double limit_price;
    int size;
    int total_volume;
    int num_orders;
    Order *head_order; 
    Order *tail_order;
    Limit *next;
    Limit *prev;
};

class OrderBook {
public:
    std::unordered_map<double, Limit*> limit_map = {};    // key is limit price
    std::unordered_map<int, Order*> order_map = {};     // key is order_id

    int num_matches {0};
    double most_recent_trade_price;
    bool debug {false};
    bool logging {true};
    
    Limit *sorted_limit_prices_head {nullptr};
    Limit *sorted_limit_prices_tail {nullptr};
    int num_limit_nodes {0}; 

    // instead of having these, we could insert a dummy "spread" limit node between best buy and best sell
    Limit *highest_buy_limit {nullptr}; 
    Limit *lowest_sell_limit {nullptr};

    void add_order(
        int order_id,
        std::string order_type,
        int shares,
        double limit,
        unsigned long long entry_time,
        unsigned long long event_time
    );
    double find_best_limit_node_to_match_with_new(std::string incoming_order_type, double incoming_order_limit);
    Limit* find_best_limit_node_to_match_with(Order *new_order_ptr);
    Limit* insert_limit_map(double limit_price, int size, int total_volume);
    void update_limit_spread_new();
    int create_match(Order *incomming_order, Limit *limit_node);
    int insert_limit_dll(Limit *new_limit);
    bool order_crossed_spread(Order *incomming_order);
    int cancel_order();
    void insert_order_dll(Order *order, Limit *limit_node);
    void print_orders_dll(Order *n);
    void print_limits_dll(Limit *n);
    void print_limits_dll_backwards(Limit *n);
    int validate();
    void update_limit_spread(Limit *limit, std::string order_type);
    friend std::ostream& operator<<(std::ostream& os, const OrderBook& book);
};
