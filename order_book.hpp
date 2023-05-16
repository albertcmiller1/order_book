#include <iostream>

// doubly LL
struct Order {
    int id_number;
    bool buy_sell;
    int shares;
    float limit;
    int entry_time;
    int event_time;
    Order* next;
    Order* prev;
    // Limit* parentLimit; //how to add this to scope?
};

// Binary Search Tree
struct Limit {
    float limit_price;
    int size;
    int total_volume;
    Limit *parent;
    Limit *left;
    Limit *right;
    Order *head_order;
    Order *tail_order;
};

class OrderBook {
public:
    std::unordered_map<int, Order> order_map = {};
    std::unordered_map<float, Limit> limit_map = {};

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