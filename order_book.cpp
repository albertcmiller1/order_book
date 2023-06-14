#include "order_book.hpp" 
using namespace std; 

void OrderBook::add_order(
        int order_id,
        std::string order_type,
        int shares,
        float limit_price,
        unsigned long long entry_time,
        unsigned long long event_time
    ){

    Order *new_order_ptr = new Order {
        order_id,
        order_type,
        shares, 
        limit_price,
        entry_time,
        event_time
    };

    // this is the wrong place to do this
    this->update_best_offers(*new_order_ptr);

    // add new order to order_map
    order_map[order_id] = new_order_ptr;

    if (this->limit_map.find(limit_price) == this->limit_map.end()) {
        // this price doesnt exist in limit map yet. create new limit node and add to limit_map + limit_dll. > could prob consolodate these two. 
        Limit& limit_node = this->insert_limit_map(limit_price, shares, shares);
        this->insert_limit_dll(&limit_node);

        // check if new limit price crosses the spread 
        // this->check_for_spread_cross(new_order_ptr, limit_node);
        // this->check_for_limit_match(new_order_ptr, limit_node);

        // add the first order to new limit node. [only do this if a match was not made.]
        this->insert_order_dll(new_order_ptr, limit_node);
    } else {
        // price exists in limit map, find it 
        auto it = limit_map.find(limit_price); 
        Limit &limit_node = it->second;  

        // if the incoming order is the opposite type of the current existing orders in the Limit's DLL, we can make a trade 
        // this->check_for_spread_cross(new_order_ptr, limit_node);
        // this->check_for_limit_match(new_order_ptr, limit_node);
        
        // else we need to just append into DLL
        this->insert_order_dll(new_order_ptr, limit_node);
    }
}

void OrderBook::update_best_offers(Order &order){
    if (order.order_type == "buy"){
        if (this->highest_buy_offer){
            if (order.limit > this->highest_buy_offer){
                this->highest_buy_offer = order.limit;
            }        
        } else {
            this->highest_buy_offer = order.limit;
        }
    }
    if (order.order_type == "sell"){
        if (this->lowest_sell_offer){
            if (order.limit < this->lowest_sell_offer){
                this->lowest_sell_offer = order.limit;
            }        
        } else {
            this->lowest_sell_offer = order.limit;
        }    
    }
}

void OrderBook::insert_order_dll(Order *order, Limit &limit_node){
    // O(1) insertion and should also guarantee the LL is ordererd from oldest to newest
    if (limit_node.head_order) {
        Order *new_node = new Order;

        new_node->order_id = order->order_id;
        new_node->order_type = order->order_type;
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
        first_node->order_type = order->order_type;
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
    this->num_limit_prices++;

    this->limit_map[limit_price] = Limit {
        limit_price,
        size,
        total_volume,
        nullptr, 
        nullptr, 
        nullptr, 
        nullptr
    };

    return limit_map[limit_price];
}

int OrderBook::insert_limit_dll(Limit *new_limit){
    if (this->sorted_limit_prices_head) {
        Limit *curr = this->sorted_limit_prices_head;
        while (curr != nullptr){
            if (!curr->prev && new_limit->limit_price < curr->limit_price){
                // inserting at head
                new_limit->prev = nullptr;
                new_limit->next = this->sorted_limit_prices_head;
                curr->prev = new_limit;
                this->sorted_limit_prices_head = new_limit;
                return 0;
            } else if (!curr->next && new_limit->limit_price > curr->limit_price) {
                // inserting at tail 
                new_limit->prev = this->sorted_limit_prices_tail;
                new_limit->next = nullptr;
                curr->next = new_limit;
                this->sorted_limit_prices_tail = new_limit;
                return 0;
            } else if (curr->limit_price < new_limit->limit_price && new_limit->limit_price < curr->next->limit_price){
                // inserting in middle
                new_limit->prev = curr;
                new_limit->next = curr->next;
                curr->next = new_limit;
                curr->next->prev = new_limit;
                return 0;
            } else {
                curr = curr->next;
            }
        }
    } else {
        // first insertion 
        this->sorted_limit_prices_head = new_limit;
        this->sorted_limit_prices_tail = new_limit;
    }

    return 0;
}

int OrderBook::validate(){
    // go through each limit price in limit_map
    // make sure all orders attached at that price are of the same order_type. 
    return 0;
}

int OrderBook::check_for_spread_cross(Order *incomming_order, Limit &limit_node){
    
    // compare against limit price's DLL
    // NOTE: this is only comparing against head order. i think sould be fine though. (all order of a Limit dll should have the same order type, and the DLL's head should always be the oldest order.)
    Order *curr_old_order = limit_node.head_order;

    std::cout << "checking if new order crosses the spread.\n";

    if (incomming_order->order_type == "buy" && incomming_order->limit > this->lowest_sell_offer){
        std::cout << "new buy order has crossed the spread ...";
        // this should only happen if the limit node associated with the new order has an empty DLL
        return 0;
    } else if (incomming_order->order_type == "sell" && incomming_order->limit < this->highest_buy_offer){
        std::cout << "new sell order has crossed the spread ...";
        // this should only happen if the limit node associated with the new order has an empty DLL
        return 0;
    } else {
        std::cout << "new order did not cross the spread\n";
        std::cout << "\n\n";
        return 0;
    }
}

int OrderBook::check_for_limit_match(Order *incomming_order, Limit &limit_node){
    
    // compare against limit price's DLL
    // NOTE: this is only comparing against head order. i think sould be fine though. (all order of a Limit dll should have the same order type, and the DLL's head should always be the oldest order.)
    Order *curr_old_order = limit_node.head_order;

    std::cout << "checking for match...\n";

    if (curr_old_order && incomming_order->order_type != curr_old_order->order_type){
        // TODO: need some kinda if here to also ensure this section of logic only get hit
        // if the spread hasnt been crossed. 

        int buyers_order_id {0};
        int sellers_order_id {0};
        srand((unsigned) time(NULL));

        if (incomming_order->order_type == "buy"){
            buyers_order_id = incomming_order->order_id;
            sellers_order_id = curr_old_order->order_id;
        } else {
            buyers_order_id = curr_old_order->order_id;
            sellers_order_id = incomming_order->order_id;
        }

        // create a new match 
        // Match new_match = Match {
        //     rand(),                     // match_id
        //     buyers_order_id,            // buying_order_id;
        //     sellers_order_id,           // selling_order_id;
        //     1,                          // sale_quantity;
        //     incomming_order->limit      // sale_price;
        // };

        if (incomming_order->shares == curr_old_order->shares){
            // remove old (matched with) order from DLL
            std::cout << "perfect match between buyer and seller" << std::endl;

            // std::cout << "gonna delete: " << curr_old_order->order_id << std::endl;
            // this->print_list(limit_node.head_order);

            // std::cout << "deleting first node... \n";
            // limit_node.head_order = curr_old_order->next;
            // limit_node.head_order->prev = nullptr;
            // // delete the order to save memroy 
            // delete curr_old_order;

            // this->print_list(limit_node.head_order);
            return 0;

        } else if (incomming_order->shares > curr_old_order->shares){
            std::cout << "incoming order wants to buy/sell more orders than the curr_old_order has. create match, delete curr_old_order, and continue trying to fill orders \n";
            std::cout << "\n\n";
            return 0;
        } else {
            // incomming_order->shares < curr_old_order->shares
            std::cout << "incoming order does not have enough shares to completely fill curr_old_order. create match, partially fill curr_old_order, update curr_old_order, delete incomming_order, return.\n";
            std::cout << "\n\n";
            return 0;
        }
    } else {
        std::cout << "no match here\n";
        std::cout << "\n\n";
        return 0;
    }
}

int OrderBook::cancel_order(){
    return 0;
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
    std::cout << "HIGHEST BUY OFFER: " << book.highest_buy_offer << std::endl;
    std::cout << "LOWEST SELL OFFER: " << book.lowest_sell_offer << std::endl;

    std::cout << "\norder_map: " << std::endl;
    std::cout << "------------" << std::endl;
    std::cout << "order_id\t   limit        qantity\t\torder_type" << std::endl;
    for (auto it = book.order_map.begin(); it != book.order_map.end(); it++){
        os << it->first << "\t   " << it->second->limit << "\t" << it->second->shares << "\t\t" << it->second->order_type << std::endl;
    }

    std::cout << "\n\nlimit_map: " << std::endl;
    std::cout << "------------" << std::endl;
    std::cout << "price      volume       num_orders \t order_ids" << std::endl;
    for (auto it = book.limit_map.begin(); it != book.limit_map.end(); it++){
        // this is O(n) -- instead, just keep a new field on Limit.num_orders
        int num_orders_at_limit_price {0};
        Order *curr = it->second.head_order;;
        while (curr != nullptr) {
            num_orders_at_limit_price++;
            curr = curr->next;
        };

        std::string s = " ";
        Order *n = it->second.head_order;
        while (n != nullptr) {
                s = s + std::to_string(n->order_id) + " ";
                n = n->next;
        };

        os << it->first << "\t   " << it->second.total_volume << "\t\t"  << num_orders_at_limit_price << "\t\t" << s << std::endl;
    }

    std::cout << "\n\nSORTED LIMIT ARRAY: \n";
    Limit *curr = book.sorted_limit_prices_head;
    while (curr != nullptr) {
        cout << curr->limit_price << " ";
        curr = curr->next;
    }

    return os;
}

