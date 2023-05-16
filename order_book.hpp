#include <iostream>

class OrderBook {
    std::unordered_map<float, int> bids, asks = {};
public:
    bool is_empty() const;
    void add_bid(float price, int quantity);
    void add_ask(float price, int quantity);

    friend std::ostream& operator<<(std::ostream& os, const OrderBook& book);
};