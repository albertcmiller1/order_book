#include "order_book.hpp" 
using namespace std; 

bool OrderBook::order_map_is_empty() const {
    return order_map.empty();
}

bool OrderBook::limit_map_is_empty() const {
    return limit_map.empty();
}

void OrderBook::add_order(
        int order_id,
        bool buy_sell,
        int shares,
        float limit_price,
        int entry_time,
        int event_time
    ){

    // add new order to order_map
    order_map[order_id] = Order {
        order_id,
        buy_sell,
        shares, 
        limit_price,
        entry_time,
        event_time
    };



    // if price in limt_map 
    if (this->limit_map.find(limit_price) == this->limit_map.end()) {
        // this price is not in the limit map -> update limit_map. 
        this->insert_limit_map(limit_price, shares, shares);
        

        // since this price was not in limit_map, we need to insert a new node in limit tree
        
        if (this->limit_tree_head){     // if tree exists, append 

            Limit *root = this->insert_limit_tree(this->limit_tree_head, limit_price);
            // Limit *node = this->search_limit_tree(root, limit_price);

            cout << "just appended a new node onto limit tree with value: " << root->limit_price << endl;
            
            // create first node of LL
            // this->insert_order_dll(order_map[order_id], node);

        } else {    // else create new tree

            Limit *root = this->OrderBook::insert_limit_tree(nullptr, limit_price);
            this->limit_tree_head=root;
            cout << "just created the root of the limit tree with value: " << root->limit_price << endl;
            // create first node of LL
            this->insert_order_dll(order_map[order_id], root);
        }


    } else {
        // this price is already in the limit_map. 
        auto it = limit_map.find(limit_price); 
        // std::cout << "found this price in limit_map:  " << std::endl;
        // std::cout << (*it).first << std::endl;
        // std::cout << (*it).second.size << std::endl;

        // Limit *node = (*it)->second;
        // go into it's existing LL and append 
        // this->insert_order_dll(order_map[order_id], node);
    }
    
}

void OrderBook::insert_order_dll(Order order, Limit* parent_limit){
    if (this->order_ll_head) {
        // start at head of list, and traverse until time fits. 
        // for now, just append to end. 
    } else {
        // first node in ll
        Order *first_node = new Order;
        first_node->order_id = order.order_id;
        first_node->buy_sell = order.buy_sell;
        first_node->shares = order.shares;
        first_node->limit = order.limit;
        first_node->entry_time = order.entry_time;
        first_node->event_time = order.event_time;

        first_node->prev = nullptr;
        first_node->next = nullptr;
        first_node->parent_limit = parent_limit;
    }
}

void OrderBook::insert_limit_map(float limit_price, int size, int total_volume){
    limit_map[limit_price] = Limit {
        limit_price,
        size,
        total_volume
    };
}

/*
Limit* OrderBook::search_limit_tree(Limit *root, int limit_price){
    if (root == nullptr || root->limit_price == limit_price)
        cout << "found node with price: " << root->limit_price << endl;
        return root;
 
    if (root->limit_price < limit_price)
        return search_limit_tree(root->right, limit_price);
 
    return search_limit_tree(root->left, limit_price);
}
*/


// this needs to return the node it just created 
Limit* OrderBook::insert_limit_tree(Limit *root, float price){
    if (!root) {
        root = new Limit;
        root->limit_price=price;
        return root;
    }
 
    if (price > root->limit_price) {
        root->right = insert_limit_tree(root->right, price);

    } else {
        root->left = insert_limit_tree(root->left, price);

    }

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
    std::cout << "order_id   limit        qantity" << std::endl;
    for (auto it = book.order_map.begin(); it != book.order_map.end(); it++){
        os << it->first << "\t   " << it->second.limit << "\t" << it->second.shares << std::endl;
    }

    std::cout << "\n\nlimit_map: " << std::endl;
    std::cout << "------------" << std::endl;
    std::cout << "price      volume       orders" << std::endl;
    for (auto it = book.limit_map.begin(); it != book.limit_map.end(); it++){
        // for loop to get all orders attached to it->second->head_order
        os << it->first << "\t   " << it->second.total_volume << "\t" << std::endl;
    }

    std::cout << "\n\nlimit_tree: " << std::endl;
    std::cout << "------------" << std::endl;
    print_tree("", book.limit_tree_head, false);
    std::cout << std::endl;
    return os;
}
