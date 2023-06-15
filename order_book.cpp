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


    // add new order to order_map
    order_map[order_id] = new_order_ptr;

    if (this->limit_map.find(limit_price) == this->limit_map.end()) {
        // this price doesnt exist in limit map yet. create new limit node and add to limit_map + limit_dll. > could prob consolodate these two. 
        Limit& limit_node = this->insert_limit_map(limit_price, shares, shares);
        this->insert_limit_dll(&limit_node);

        // check if new limit price crosses the spread 
        this->order_crossed_spread(new_order_ptr, limit_node);

        // add the first order to new limit node. [only do this if a match was not made.]
        this->insert_order_dll(new_order_ptr, limit_node);

        // this is the wrong place to do this
        this->update_limit_spread(limit_node, new_order_ptr->order_type);
    } else {
        // price exists in limit map, find it 
        auto it = limit_map.find(limit_price); 
        Limit &limit_node = it->second;  

        if (!this->order_crossed_spread(new_order_ptr, limit_node)){
            this->insert_order_dll(new_order_ptr, limit_node);
        } else {
            std::cout << "match has been created \n";
        }
        
        // this is the wrong place to do this
        this->update_limit_spread(limit_node, new_order_ptr->order_type);
    }
}

void OrderBook::update_limit_spread(Limit &limit, std::string order_type){
    if (order_type == "buy"){
        if (this->highest_buy_limit){
            if (limit.limit_price > this->highest_buy_limit->limit_price){
                this->highest_buy_limit = &limit;
            }        
        } else {
            this->highest_buy_limit = &limit;
        }
    }
    if (order_type == "sell"){
        if (this->lowest_sell_limit){
            if (limit.limit_price < this->lowest_sell_limit->limit_price){
                this->lowest_sell_limit = &limit;
            }        
        } else {
            this->lowest_sell_limit = &limit;
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
    this->num_limit_nodes++;

    this->limit_map[limit_price] = Limit {
        limit_price,
        size,
        total_volume,
        1,
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

bool OrderBook::order_crossed_spread(Order *incomming_order, Limit &limit_node){
    
    // compare against limit price's DLL
    // NOTE: this is only comparing against head order. i think sould be fine though. (all order of a Limit dll should have the same order type, and the DLL's head should always be the oldest order.)

    if (!this->lowest_sell_limit || !this->highest_buy_limit){
        // cant compare this buy/sell order bc we dont have both buy and sell orders yet
        return false;
    } else if (incomming_order->order_type == "buy" && incomming_order->limit >= this->lowest_sell_limit->limit_price){
        std::cout << "new buy order has crossed the spread ...\n";
        // what to do if order has a very different limit price and we need to adjust the limit_node? 
        this->create_match(incomming_order, limit_node);
        return true;
    } else if (incomming_order->order_type == "sell" && incomming_order->limit <= this->highest_buy_limit->limit_price){
        std::cout << "new sell order has crossed the spread ...\n";
        // what to do if order has a very different limit price and we need to adjust the limit_node? 
        this->create_match(incomming_order, limit_node);
        return true;
    } else {
        // new order did not cross the spread
        return false;
    }
}

int OrderBook::create_match(Order *incomming_order, Limit &limit_node){
    std::cout << "attempting to create a match...\n";

    if (limit_node.head_order && incomming_order->order_type != limit_node.head_order->order_type){
        int buyers_order_id {0};
        int sellers_order_id {0};
        srand((unsigned) time(NULL));

        // set buyer and seller
        if (incomming_order->order_type == "buy"){
            buyers_order_id = incomming_order->order_id;
            sellers_order_id = limit_node.head_order->order_id;
        } else {
            buyers_order_id = limit_node.head_order->order_id;
            sellers_order_id = incomming_order->order_id;
        }

        if (incomming_order->shares == limit_node.head_order->shares){
            // remove old (matched with) order from DLL
            std::cout << "perfect match between buyer (" << buyers_order_id << ") and seller (" << sellers_order_id << ")"  << std::endl;

            this->print_list(limit_node.head_order);

            // create a new match 
            Match new_match = Match {
                rand(),                     // match_id
                buyers_order_id,            // buying_order_id;
                sellers_order_id,           // selling_order_id;
                1,                          // sale_quantity;
                incomming_order->limit      // sale_price;
            };

            // delete head 
            std::cout << "deleting old order..." << limit_node.head_order->order_id << std::endl;
            Order *tmp = limit_node.head_order;
            limit_node.head_order = limit_node.head_order->next;
            if (limit_node.head_order) { 
                limit_node.head_order->prev=nullptr; 
            }
            this->order_map.erase(tmp->order_id);
            delete tmp;

            // delete incomming_order.
            std::cout << "deleting incoming order... " << incomming_order->order_id << std::endl;
            this->order_map.erase(incomming_order->order_id);
            delete incomming_order;

            if (!limit_node.head_order){
                // if limit has no orders, delete it
                std::cout << "DELETING LIMIT NODE\n";
                limit_node.prev->next = limit_node.next;
                limit_node.next->prev = limit_node.prev;
                this->limit_map.erase(limit_node.limit_price);
                // delete &limit_node; // dont need to delete bc limit_node is on stack? 
            }

            this->print_list(limit_node.head_order);
            return 0;

        } else if (incomming_order->shares > limit_node.head_order->shares){
            std::cout << "incoming order wants to " << incomming_order->order_type << " more orders than the limit_node.head_order has. create match, delete limit_node.head_order, and continue trying to fill orders \n";

            std::cout << "_________original incoming order____________" << std::endl;
            std::cout << "shares: " << incomming_order->shares << std::endl;
            std::cout << "order_type: " << incomming_order->order_type << std::endl;
            std::cout << "limit: " << incomming_order->limit << std::endl;
            std::cout << "order_id: " << incomming_order->order_id << std::endl;
            std::cout << "_______________________________" << std::endl;

            while (limit_node.head_order && incomming_order->shares > 0){
                std::cout << "STARTING LOOP\n" << std::endl;
                
                // create a new order which will match the quantity of the limit_node.head_order 
                srand((unsigned) time(NULL));
                int order_id = rand();
                Order *new_order_ptr = new Order {
                    order_id,                           // order_id
                    incomming_order->order_type,        // order_type
                    limit_node.head_order->shares,      // shares
                    incomming_order->limit,             // limit
                    incomming_order->entry_time,        // entry_time
                    incomming_order->entry_time         // event_time
                };

                // add new order to order_map
                order_map[order_id] = new_order_ptr;

                // update the incoming order (subtract the shares)
                incomming_order->shares -= limit_node.head_order->shares;

                std::cout << "_____original incoming order after update____" << std::endl;
                std::cout << "shares: " << incomming_order->shares << std::endl;
                std::cout << "order_type: " << incomming_order->order_type << std::endl;
                std::cout << "limit: " << incomming_order->limit << std::endl;
                std::cout << "order_id: " << incomming_order->order_id << std::endl;
                std::cout << "_______________________________" << std::endl;


                std::cout << "_____new branched order from incoming order____" << std::endl;
                std::cout << "shares: " << new_order_ptr->shares << std::endl;
                std::cout << "order_type: " << new_order_ptr->order_type << std::endl;
                std::cout << "limit: " << new_order_ptr->limit << std::endl;
                std::cout << "order_id: " << new_order_ptr->order_id << std::endl;
                std::cout << "_______________________________" << std::endl;
                
                
                // recursion to create match 
                std::cout << "... starting recursion ....\n";
                this->create_match(new_order_ptr, limit_node);
                std::cout << "... past recursion ....\n";

                std::cout << "ENDING LOOP\n\n\n\n\n" << std::endl;
            }
            
            std::cout << "OUTSIDE LOOP\n\n\n\n\n" << std::endl;


            std::cout << "_____original incoming order after update____" << std::endl;
            std::cout << "shares: " << incomming_order->shares << std::endl;
            std::cout << "order_type: " << incomming_order->order_type << std::endl;
            std::cout << "limit: " << incomming_order->limit << std::endl;
            std::cout << "order_id: " << incomming_order->order_id << std::endl;
            std::cout << "_______________________________" << std::endl;
            

            if (incomming_order->shares == 0){
                std::cout << "deleting order: " << incomming_order->order_id << std::endl;
                this->order_map.erase(incomming_order->order_id);
                delete incomming_order;
            } else {
                // change limit nodes and keep going 
                std::cout << "changing limit nodes and trying again\n" << std::endl;
                // this->create_match(incomming_order, *limit_node.next);
            }


            // if after all this the order still persits, we need to create it a limit node. 
            // i think this should only happen if the initial order was massive and eats up all the others 

            std::cout << "\n\n";
            return 0;
        } else {
            // incomming_order->shares < limit_node.head_order->shares
            std::cout << "incoming order does not have enough shares to completely fill limit_node.head_order. create match, partially fill limit_node.head_order, update limit_node.head_order, delete incomming_order, return.\n";
            std::cout << "\n\n";
            return 0;
        }
    } else {
        std::cout << "this should never happen! ???\n";
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
    std::cout << "HIGHEST BUY OFFER: " << book.highest_buy_limit->limit_price << std::endl;
    std::cout << "LOWEST SELL OFFER: " << book.lowest_sell_limit->limit_price << std::endl;

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

