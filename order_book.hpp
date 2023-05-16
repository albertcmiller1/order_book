#include <iostream>

// class Order {
//     int id_number;
//     bool buy_sell;
//     int shares;
//     float limit;
//     int entry_time;
//     int event_time;
// };


class OrderBook {
public:

    struct Order {
        int id_number;
        bool buy_sell;
        int shares;
        float limit;
        int entry_time;
        int event_time;
    };

    struct Limit {
        float limit_price;
        int size;
        int total_volume;
    };

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

    // friend std::ostream& operator<<(std::ostream& os, const OrderBook& book);
};