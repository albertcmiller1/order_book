#include <iostream>
#include <iomanip>

struct Limit;

struct Match {
    std::string match_id;
    int buying_order_id;
    int selling_order_id;
    int sale_quantity;
    float sale_price;
};

// doubly LL
struct Order {
    int order_id;
    bool buy_sell;
    int shares;
    float limit;
    int entry_time;
    int event_time;
    Order* next {nullptr};
    Order* prev {nullptr};
};

struct Limit {
    float limit_price;
    int size;
    int total_volume;
    Order *head_order; 
    Order *tail_order;
};

class OrderBook {
public:
    std::unordered_map<float, Limit> limit_map = {};    // key is limit price
    std::unordered_map<int, Order> order_map = {};      // key is order_id

    void add_order(
        int order_id,
        bool buy_sell,
        int shares,
        float limit,
        int entry_time,
        int event_time
    );

    Limit& insert_limit_map(
        float limit_price,
        int size,
        int total_volume
    );

    void insert_order_dll(Order order, Limit &limit_node);
    void print_list(Order *n);
    friend std::ostream& operator<<(std::ostream& os, const OrderBook& book);
};


