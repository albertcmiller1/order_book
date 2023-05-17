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
        float limit_price,
        int entry_time,
        int event_time
    ){

    // add new order to order_map
    order_map[id_number] = Order {
        id_number,
        buy_sell,
        shares, 
        limit_price,
        entry_time,
        event_time
    };

    // if price in limt_map 
    if (this->limit_map.find(limit_price) == this->limit_map.end()) {
        // this price is not in the limit map-- update limit_map. 
        this->OrderBook::add_limit(limit_price, shares, shares);
    } else {
        // this price is already in the limit_map. 
        auto it = limit_map.find(limit_price); 
        std::cout << "found this price in limit_map:  " << std::endl;
        std::cout << (*it).first << std::endl;
        std::cout << (*it).second.size << std::endl;
        // go into it's LL and append 
    }
    
    // update limit_tree
    if (this->limit_tree_head){
        Limit *root = this->OrderBook::insert_new_limit(this->limit_tree_head, limit_price);
    } else {
        Limit *root = this->OrderBook::insert_new_limit(nullptr, limit_price);
        this->limit_tree_head=root;
    }
};

void OrderBook::add_limit(
        float limit_price,
        int size,
        int total_volume
    ){

    limit_map[limit_price] = Limit {
        limit_price,
        size,
        total_volume
    };
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

void print_tree(const std::string& prefix, const Limit* node, bool isLeft){
    if(node != nullptr){
        std::cout << prefix;
        std::cout << (isLeft ? "├──" : "└──" );
        std::cout << node->limit_price << std::endl;
        print_tree( prefix + (isLeft ? "│   " : "    "), node->left, true);
        print_tree( prefix + (isLeft ? "│   " : "    "), node->right, false);
    }
}

std::ostream& operator<<(std::ostream& os, const OrderBook& book){
    cout << "\n";
    if (book.order_map_is_empty()){
        os << "ORDER MAP EMPTY";
        return os;
    }
    std::cout << "order_map: " << std::endl;
    std::cout << "------------" << std::endl;
    std::cout << "order_id   limit  qantity" << std::endl;
    for (auto it = book.order_map.begin(); it != book.order_map.end(); it++){
        os << it->first << "\t   " << it->second.limit << "\t" << it->second.shares << std::endl;
    }

    std::cout << "\n\nlimit_map: " << std::endl;
    std::cout << "------------" << std::endl;
    std::cout << "price      volume" << std::endl;
    for (auto it = book.limit_map.begin(); it != book.limit_map.end(); it++){
        os << it->first << "\t   " << it->second.total_volume << "\t" << std::endl;
    }

    std::cout << "\n\nlimit_tree: " << std::endl;
    std::cout << "------------" << std::endl;
    print_tree("", book.limit_tree_head, false);
    std::cout << std::endl;
    return os;
}
