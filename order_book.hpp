#include <iostream>
#include <iomanip>
#include <unistd.h>  
#include <cstdlib>
#include <chrono>
#include <vector> 

struct Limit;

struct Match {
    int match_id;
    int buying_order_id;
    int selling_order_id;
    int sale_quantity;
    float sale_price;
};

// doubly LL
struct Order {
    int order_id;
    std::string order_type;
    int shares;
    float limit;
    unsigned long long entry_time;
    unsigned long long event_time;
    Order* next {nullptr};
    Order* prev {nullptr};
};

struct Limit {
    float limit_price;
    int size;
    int total_volume;
    Order *head_order; 
    Order *tail_order;
    Limit *next;
    Limit *prev;
};

class OrderBook {
public:
    std::unordered_map<float, Limit> limit_map = {};    // key is limit price
    std::unordered_map<int, Order*> order_map = {};     // key is order_id
    
    Limit *sorted_limit_prices_head {nullptr};
    Limit *sorted_limit_prices_tail {nullptr};
    int num_limit_prices {0}; 

    // can either be their price, or a pointer to the actual node
    float highest_buy_offer {0.00}; // could just be an int of the index of the sorted_limit_prices vector 
    float lowest_sell_offer {0.00};

    void add_order(
        int order_id,
        std::string order_type,
        int shares,
        float limit,
        unsigned long long entry_time,
        unsigned long long event_time
    );

    Limit& insert_limit_map(
        float limit_price,
        int size,
        int total_volume
    );

    int insert_limit_dll(Limit *new_limit);
    int check_for_spread_cross(Order *incomming_order, Limit &limit_node);
    int check_for_limit_match(Order *incomming_order, Limit &limit_node);
    int cancel_order();
    void insert_order_dll(Order *order, Limit &limit_node);
    void print_list(Order *n);
    int validate();
    void update_best_offers(Order &order);
    friend std::ostream& operator<<(std::ostream& os, const OrderBook& book);
};


