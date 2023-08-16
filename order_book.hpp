#include <iostream>
#include <iomanip>
#include <unistd.h>  
#include <cstdlib>
#include <chrono>
#include <vector> 
#include <stdexcept>
#include <random>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include "crow.h"

struct Match {
    int match_id;
    std::string buying_order_id;
    std::string selling_order_id;
    int sale_quantity;
    double sale_price;
};

struct Order {
    std::string order_id;
    std::string order_type;
    // int customer_id;
    int shares;
    double limit;
    uint64_t entry_time;
    uint64_t event_time;
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
    std::unordered_map<double, Limit*> limit_map = {};          // key is limit price
    std::unordered_map<std::string, Order*> order_map = {};     // key is order_id
    std::unordered_set<crow::websocket::connection*> users;

    int num_matches {0};
    double most_recent_trade_price {0};
    bool debug {true};
    bool logging {true};
    
    Limit *sorted_limit_prices_head {nullptr};
    Limit *sorted_limit_prices_tail {nullptr};
    int num_limit_nodes {0}; 

    Limit *highest_buy_limit {nullptr}; 
    Limit *lowest_sell_limit {nullptr};

    void add_order(std::string order_id, std::string order_type, int shares, double limit, uint64_t entry_time, uint64_t event_time);
    double find_best_limit_node_to_match_with_new(std::string incoming_order_type, double incoming_order_limit);
    Limit* find_best_limit_node_to_match_with(Order *new_order_ptr);
    Limit* insert_limit_map(double limit_price, int size, int total_volume);
    void update_limit_spread_new();
    void broadcast_to_users(std::string message);

    void create_match(Order *incomming_order, Limit *limit_node);
    void perfect_match(Order *incomming_order, Limit *limit_node, std::string buyers_order_id, std::string sellers_order_id);
    void branch_from_incoming_order(Order *incomming_order, Limit *limit_node, Limit *tmp_prev, Limit *tmp_next);
    void branch_from_existing_order(Order *incomming_order, Limit *limit_node);

    int insert_limit_dll(Limit *new_limit);
    bool order_crossed_spread(Order *incomming_order);
    int cancel_order();
    void insert_order_dll(Order *order, Limit *limit_node);
    void print_orders_dll(Order *n);
    void print_limits_dll();
    void print_limits_dll_backwards();
    int validate();
    void update_limit_spread_old(Limit *limit, std::string order_type);
    std::string generate_order_id();
    friend std::ostream& operator<<(std::ostream& os, const OrderBook& book);
};
