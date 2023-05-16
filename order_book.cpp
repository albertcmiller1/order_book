#include "order_book.hpp" 
using namespace std; 

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

Limit* OrderBook::insert_new_limit(Limit *root, float price){

    if (!root) {
        root = new Limit;
        root->limit_price=price;
        return root;
    }
 
    if (price > root->limit_price) {
        root->right = insert_new_limit(root->right, price);
    }
   
    else if (price < root->limit_price) {
        root->left = insert_new_limit(root->left, price);
    }

    // if price == limint_price, insert in Order LL
 
    return root;
}



std::ostream& operator<<(std::ostream& os, const OrderBook& book){
    if (book.order_map_is_empty()){
        os << "ORDER MAP EMPTY";
        return os;
    }

    std::cout << "order_id   limit  qantity" << std::endl;
    for (auto it = book.order_map.begin(); it != book.order_map.end(); it++){
        os << it->first << "\t   " << it->second.limit << "\t" << it->second.shares << std::endl;
    }
    
    std::cout << std::endl;
    return os;
}