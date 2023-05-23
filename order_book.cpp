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

    if (this->limit_map.find(limit_price) == this->limit_map.end()) {
        Limit& limit_node = this->insert_limit_map(limit_price, shares, shares);
        this->insert_order_dll(order_map[order_id], limit_node);
    } else {
        auto it = limit_map.find(limit_price); 
        Limit &limit_node = it->second;  
        this->insert_order_dll(order_map[order_id], limit_node);
    }
}

void OrderBook::insert_order_dll(Order order, Limit &limit_node){
    if (limit_node.head_order) {
        Order *new_node = new Order;

        new_node->order_id = order.order_id;
        new_node->buy_sell = order.buy_sell;
        new_node->shares = order.shares;
        new_node->limit = order.limit;
        new_node->entry_time = order.entry_time;
        new_node->event_time = order.event_time;

        limit_node.tail_order->next = new_node;
        new_node->prev = limit_node.tail_order;
        new_node->next = nullptr;

        limit_node.tail_order = new_node;
    } else {
        Order *first_node = new Order;
        
        first_node->order_id = order.order_id;
        first_node->buy_sell = order.buy_sell;
        first_node->shares = order.shares;
        first_node->limit = order.limit;
        first_node->entry_time = order.entry_time;
        first_node->event_time = order.event_time;

        first_node->prev = nullptr;
        first_node->next = nullptr;

        limit_node.head_order = first_node;
        limit_node.tail_order = first_node;
    }
}

Limit& OrderBook::insert_limit_map(float limit_price, int size, int total_volume){
    limit_map[limit_price] = Limit {
        limit_price,
        size,
        total_volume
    };

    return limit_map[limit_price];
}

void OrderBook::print_list(Order *n) {
    cout << "\nPrinting list..." << endl;
    while (n != nullptr) {
        cout << n->order_id << " ";
        n = n->next;
    }
    cout << "done...\n\n" << endl;
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
        // this is O(n) -- instead, just keep a new field on Limit.num_orders
        int num_orders_at_limit_price {0};
        Order *curr = it->second.head_order;;
        while (curr != nullptr) {
            num_orders_at_limit_price++;
            curr = curr->next;
        }
        os << it->first << "\t   " << it->second.total_volume << "\t\t"  << num_orders_at_limit_price << std::endl;
    }

    return os;
}
