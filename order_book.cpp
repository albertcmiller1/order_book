#include "order_book.hpp" 

bool OrderBook::order_map_is_empty() const {
    return order_map.empty();
}

bool OrderBook::limit_map_is_empty() const {
    return limit_map.empty();
}

void OrderBook::add_order(
        int id_number,
        bool buy_sell,
        int shares,
        float limit,
        int entry_time,
        int event_time
    ){
    Order x = {
        id_number,
        buy_sell,
        shares, 
        limit,
        entry_time,
        event_time
    };
    order_map[id_number] = x;
};

void OrderBook::add_limit(
        float limit_price,
        int size,
        int total_volume
    ){

    Limit x = {
        limit_price,
        size,
        total_volume
    };

    limit_map[limit_price] = x;
}

// std::ostream& operator<<(std::ostream& os, const OrderBook& book){
//     if (book.is_empty()){
//         os << "ORDER BOOK EMPTY";
//         return os;
//     }

//     std::cout << "ask     qantity" << std::endl;
//     for (auto it = book.asks.begin(); it != book.asks.end(); it++){
//         os << it->first << "\t" << it->second << std::endl;
//     }
    
//     std::cout << std::endl;

//     for (auto it = book.bids.begin(); it != book.bids.end(); it++){
//         os << it->first << "\t" << it->second << std::endl;
//     }
//     std::cout << "bid     qantity" << std::endl;

//     return os;
// }