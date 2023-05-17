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


    // add new order to order_map
    order_map[id_number] = Order {
        id_number,
        buy_sell,
        shares, 
        limit,
        entry_time,
        event_time
    };
    // check if limit price is in limit_map
    // if so, update tree (but faster)
    
    // update limit_tree
    if (this->limit_tree_head){
        Limit *root = this->OrderBook::insert_new_limit(this->limit_tree_head, limit);
    } else {
        Limit *root = this->OrderBook::insert_new_limit(nullptr, limit);
        this->limit_tree_head=root;
    }

    // check if price is in binary tree
    // if so, update LL
    // if not, create LL
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

void print_tree(Limit* p, int indent){
    if(p != nullptr) {
        if(p->right) {
            print_tree(p->right, indent+4);
        }
        if (indent) {
            std::cout << std::setw(indent) << ' ';
        }
        if (p->right) std::cout<<" /\n" << std::setw(indent) << ' ';
        std::cout<< p->limit_price << "\n ";
        if(p->left) {
            std::cout << std::setw(indent) << ' ' <<" \\\n";
            print_tree(p->left, indent+4);
        }
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
    std::cout << "\n\nlimit_tree: " << std::endl;
    std::cout << "------------" << std::endl;
    print_tree(book.limit_tree_head, 4);
    std::cout << std::endl;
    return os;
}
