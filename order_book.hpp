#include <iostream>
#include <iomanip>

struct Limit;

// doubly LL
struct Order {
    int id_number;
    bool buy_sell;
    int shares;
    float limit;
    int entry_time;
    int event_time;
    Order* next {nullptr};
    Order* prev {nullptr};
    Limit* parent_limit {nullptr};
};

// Binary Search Tree
struct Limit {
    float limit_price;
    int size;
    int total_volume;
    Limit *parent {nullptr};
    Limit *left {nullptr};
    Limit *right {nullptr};
    Order *head_order {nullptr};
    Order *tail_order {nullptr};
};

class OrderBook {
public:
    Limit *limit_tree_head {nullptr};
    Order *order_ll_head {nullptr};
    Order *order_ll_tail {nullptr};
    std::unordered_map<int, Order> order_map = {}; // key is id_number
    std::unordered_map<float, Limit> limit_map = {}; // key is limit price

    void add_order(
        int id_number,
        bool buy_sell,
        int shares,
        float limit,
        int entry_time,
        int event_time
    );

    void add_limit(
        float limit_price,
        int size,
        int total_volume
    );

    bool order_map_is_empty() const;
    bool limit_map_is_empty() const;

    void insert_new_order();
    Limit* insert_new_limit(Limit *root, float value);

    
    friend std::ostream& operator<<(std::ostream& os, const OrderBook& book);
};

// void print_tree(std::string prefix, Limit* root, bool isLeft);

