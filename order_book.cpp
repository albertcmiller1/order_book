#include "order_book.hpp" 
using namespace std; 

void OrderBook::add_order(
        int order_id,
        std::string buy_sell,
        int shares,
        float limit_price,
        unsigned long long entry_time,
        unsigned long long event_time
    ){

    // add new order to order_map
    
    Order *new_order_ptr = new Order {
        order_id,
        buy_sell,
        shares, 
        limit_price,
        entry_time,
        event_time
        // nullptr,
        // nullptr
    };

    order_map[order_id] = new_order_ptr;

    if (this->limit_map.find(limit_price) == this->limit_map.end()) {
        // this price doesnt exist in limit map yet. create new limit node and add to limit_map. 
        Limit& limit_node = this->insert_limit_map(limit_price, shares, shares);

        // add the incoming order to new limit node. 
        this->insert_order_dll(new_order_ptr, limit_node);
    } else {
        // price exists in limit map, find it 
        auto it = limit_map.find(limit_price); 
        Limit &limit_node = it->second;  


        // if the incoming order is the opposite type of the current existing orders in the Limit's DLL, we can make a trade 
        // NOTE: also could make a trade if buy offer > sell offer ... but how do we know ? 
        this->check_for_match(new_order_ptr, limit_node);

        // else we need to just append into DLL
        this->insert_order_dll(new_order_ptr, limit_node);
    }
}

void OrderBook::insert_order_dll(Order *order, Limit &limit_node){
    // O(1) insertion and should also guarantee the LL is ordererd from newest to oldest 
    if (limit_node.head_order) {
        Order *new_node = new Order;

        new_node->order_id = order->order_id;
        new_node->buy_sell = order->buy_sell;
        new_node->shares = order->shares;
        new_node->limit = order->limit;
        new_node->entry_time = order->entry_time;
        new_node->event_time = order->event_time;

        limit_node.tail_order->next = new_node;
        new_node->prev = limit_node.tail_order;
        new_node->next = nullptr;

        limit_node.tail_order = new_node;
    } else {
        Order *first_node = new Order;
        
        first_node->order_id = order->order_id;
        first_node->buy_sell = order->buy_sell;
        first_node->shares = order->shares;
        first_node->limit = order->limit;
        first_node->entry_time = order->entry_time;
        first_node->event_time = order->event_time;

        first_node->prev = nullptr;
        first_node->next = nullptr;

        limit_node.head_order = first_node;
        limit_node.tail_order = first_node;
    }
}

Limit& OrderBook::insert_limit_map(float limit_price, int size, int total_volume){
    // TODO: update lowest_buy_offer and highest_sell_offer

    limit_map[limit_price] = Limit {
        limit_price,
        size,
        total_volume
    };

    return limit_map[limit_price];
}

void OrderBook::check_for_match(Order *incomming_order, Limit &limit_node){
    std::cout << "checking for match\n";
    std::cout << "incoming order's order_id: " << incomming_order->order_id << std::endl;
    std::cout << "incoming order's order type: " << incomming_order->buy_sell << std::endl;
    
    std::cout << "existing limit_node's head order's order_type: " << limit_node.head_order->buy_sell << std::endl;


    // compare against limit price's DLL
    // NOTE: this is only comparing against head order. i think sould be fine though. (all order of a Limit dll should have the same order type, and the DLL's head should always be the oldest order.)
    if (incomming_order->buy_sell != limit_node.head_order->buy_sell){
        std::cout << "MATCH ALERT!!!\n";
        int buyers_order_id {0};
        int sellers_order_id {0};
        srand((unsigned) time(NULL));

        if (incomming_order->buy_sell == "buy"){
            buyers_order_id = incomming_order->order_id;
            sellers_order_id = limit_node.head_order->order_id;
        } else {
            buyers_order_id = limit_node.head_order->order_id;
            sellers_order_id = incomming_order->order_id;
        }

        // create a new match 
        Match new_match = Match {
            rand(),                     // match_id
            buyers_order_id,            // buying_order_id;
            sellers_order_id,           // selling_order_id;
            1,                          // sale_quantity;
            incomming_order->limit       // sale_price;
        };
        std::cout << "new match: \n";
        std::cout << new_match.match_id << std::endl;
        std::cout << new_match.buying_order_id << std::endl;
        std::cout << new_match.selling_order_id << std::endl;

        // remove old (matched with) order from DLL
        // delete the order to save memroy 
    }

    // TODO: compare against lowest_buy_offer and highest_sell_offer

    std::cout << "\n\n";
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
    if (book.order_map.empty()){
        os << "ORDER MAP EMPTY";
        return os;
    }

    std::cout << "order_map: " << std::endl;
    std::cout << "------------" << std::endl;
    std::cout << "order_id\t   limit        qantity\t\tbuy-sell" << std::endl;
    for (auto it = book.order_map.begin(); it != book.order_map.end(); it++){
        os << it->first << "\t   " << it->second->limit << "\t" << it->second->shares << "\t\t" << it->second->buy_sell << std::endl;
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
