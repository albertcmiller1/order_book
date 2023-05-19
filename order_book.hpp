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
    std::unordered_map<int, Order> order_map = {}; // key is order_id
    std::unordered_map<float, Limit> limit_map = {}; // key is limit price

    bool order_map_is_empty() const;
    bool limit_map_is_empty() const;

    void add_order(
        int order_id,
        bool buy_sell,
        int shares,
        float limit,
        int entry_time,
        int event_time
    );

    void insert_limit_map(
        float limit_price,
        int size,
        int total_volume
    );


    void insert_order_dll(Order order, Limit* parent_limit);
    
    Limit* search_limit_tree(Limit *root, int limit_price);
    Limit* insert_limit_tree(Limit *root, float value);

    friend std::ostream& operator<<(std::ostream& os, const OrderBook& book);
};

// void print_tree(std::string prefix, Limit* root, bool isLeft);

