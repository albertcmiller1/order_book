#include "order_book.hpp" 

bool OrderBook::is_empty() const {
    return bids.empty() && asks.empty();
}

void OrderBook::add_bid(float price, int quantity){
    bids[price] += quantity;
}

void OrderBook::add_ask(float price, int quantity){
    asks[price] += quantity;
}

std::ostream& operator<<(std::ostream& os, const OrderBook& book){
    if (book.is_empty()){
        os << "ORDER BOOK EMPTY";
        return os;
    }

    std::cout << "ask     qantity" << std::endl;
    for (auto it = book.asks.begin(); it != book.asks.end(); it++){
        os << it->first << "\t" << it->second << std::endl;
    }
    
    std::cout << std::endl;

    for (auto it = book.bids.begin(); it != book.bids.end(); it++){
        os << it->first << "\t" << it->second << std::endl;
    }
    std::cout << "bid     qantity" << std::endl;

    return os;
}