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

    // std::cout << "_________incoming order____________" << std::endl;
    // std::cout << "shares: " << new_order_ptr->shares << std::endl;
    // std::cout << "order_type: " << new_order_ptr->order_type << std::endl;
    // std::cout << "limit: " << new_order_ptr->limit << std::endl;
    // std::cout << "order_id: " << new_order_ptr->order_id << std::endl;
    // std::cout << "_______________________________" << std::endl;
    
    // add new order to order_map
    order_map[order_id] = new_order_ptr;

    if (this->limit_map.find(limit_price) == this->limit_map.end()) {
        // this price doesnt exist in limit map yet. create new limit node and add to limit_map + limit_dll. > could prob consolodate these two. 
        Limit& limit_node = this->insert_limit_map(limit_price, shares, shares);
        this->insert_limit_dll(&limit_node);

        // check if new limit price crosses the spread 
        this->order_crossed_spread(new_order_ptr, limit_node);

        // prob need to do something similar as below 
        // add the first order to new limit node. [only do this if a match was not made.]
        this->insert_order_dll(new_order_ptr, limit_node);

        // this is the wrong place to do this
        // this->update_limit_spread(limit_node, new_order_ptr->order_type);
        this->update_limit_spread_new();
    } else {
        // price exists in limit map, find it 
        auto it = limit_map.find(limit_price); 
        Limit &limit_node = it->second;  

        if (!this->order_crossed_spread(new_order_ptr, limit_node)){
            this->insert_order_dll(new_order_ptr, limit_node);
        } 
        
        // this is the wrong place to do this
        // this->update_limit_spread(limit_node, new_order_ptr->order_type);
        this->update_limit_spread_new();
    }
}

void OrderBook::update_limit_spread_new(){
    // this is very inificent (but safe)
    this->highest_buy_limit = nullptr;
    this->lowest_sell_limit = nullptr;
    Limit *curr = this->sorted_limit_prices_head;

    while (curr != nullptr) {
        // loop over all limits 
        Order *n = curr->head_order;

        bool all_same = true;
        std::string first_order_type = n->order_type;

        while (n != nullptr) {
            // loop over limit's orders from oldest to newest 
            if (n->order_type != first_order_type){
                all_same = false;
            }
            n = n->next;
        };

        if (!all_same){
            cout << "??????AGGHH\n" << endl;

        } else if (this->highest_buy_limit == nullptr && curr->head_order->order_type == "buy"){
            this->highest_buy_limit = curr;
        
        } else if (this->lowest_sell_limit == nullptr && curr->head_order->order_type == "sell"){
            this->lowest_sell_limit = curr;

        } else if (curr->head_order->order_type == "buy" && curr->limit_price > this->highest_buy_limit->limit_price) {
            this->highest_buy_limit = curr;

        } else if (curr->head_order->order_type == "sell" && curr->limit_price < this->lowest_sell_limit->limit_price){
            this->lowest_sell_limit = curr;

        } else {
            // cout << "when should this happen? should only occur if the spread cant be updated\n";
            // if (this->highest_buy_limit){
            //     cout << " >> this->highest_buy_limit->limit_price: " << this->highest_buy_limit->limit_price << endl;
            // } else {
            //     cout << " >> highest_buy_limit hasnt been set yet\n";
            // }
            // if (this->lowest_sell_limit){
            //     cout << " >> this->lowest_sell_limit->limit_price: " << this->lowest_sell_limit->limit_price << endl;
            // } else {
            //     cout << " >> lowest_sell_limit hasnt been set yet\n";
            // }
            // cout << endl;
        }

        curr = curr->next;
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
        if (this->logging) std::cout << "new buy order has crossed the spread ...\n";
        this->create_match(incomming_order, limit_node);
        return true;
    } else if (incomming_order->order_type == "sell" && incomming_order->limit <= this->highest_buy_limit->limit_price){
        if (this->logging) std::cout << "new sell order has crossed the spread ...\n";
        this->create_match(incomming_order, limit_node);
        return true;
    } else {
        // new order did not cross the spread
        return false;
    }
}

int OrderBook::create_match(Order *incomming_order, Limit &limit_node){
    if (this->logging) std::cout << "attempting to create a match...\n";
    Limit *tmp_prev = limit_node.prev;
    Limit *tmp_next = limit_node.next;

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
            if (this->logging) std::cout << "perfect match between buyer (" << buyers_order_id << ") and seller (" << sellers_order_id << ")"  << std::endl;

            // create a new match 
            Match new_match = Match {
                rand(),                     // match_id
                buyers_order_id,            // buying_order_id;
                sellers_order_id,           // selling_order_id;
                1,                          // sale_quantity;
                incomming_order->limit      // sale_price;
            };

            // delete head 
            if (this->logging) std::cout << "deleting old order..." << limit_node.head_order->order_id << std::endl;
            Order *tmp = limit_node.head_order;
            limit_node.head_order = limit_node.head_order->next;
            if (limit_node.head_order) { 
                limit_node.head_order->prev=nullptr; 
            }
            this->order_map.erase(tmp->order_id);
            delete tmp;

            // delete incomming_order.
            if (this->logging) std::cout << "deleting incoming order... " << incomming_order->order_id << std::endl;
            this->order_map.erase(incomming_order->order_id);
            delete incomming_order;

            if (!limit_node.head_order){
                // if limit has no orders, delete it
                if (this->logging) std::cout << "\nlimit node is all out of orders! DELETING LIMIT NODE\n";

                if (limit_node.prev && limit_node.next){
                    limit_node.prev->next = limit_node.next;
                    limit_node.next->prev = limit_node.prev;
                    this->limit_map.erase(limit_node.limit_price);
                } else if (limit_node.prev && !limit_node.next){
                    // tail 
                    limit_node.prev->next = nullptr;
                    this->limit_map.erase(limit_node.limit_price);
                }
                
                this->update_limit_spread_new();
                // delete &limit_node; // dont need to delete bc limit_node is on stack? 
            }

            return 0;

        } else if (incomming_order->shares > limit_node.head_order->shares){
            if (this->logging) std::cout << "incoming order wants to " << incomming_order->order_type << " more orders than the limit_node.head_order has. create match, delete limit_node.head_order, and continue trying to fill orders \n";

            if (this->logging) std::cout << "STARTING traversing limit node (" << limit_node.limit_price << ") orders to create matches.\n" << std::endl;
            while (limit_node.head_order && incomming_order->shares > limit_node.head_order->shares){
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
                incomming_order->shares -= limit_node.head_order->shares; 
                if (incomming_order->shares < 0){
                    cout << "SOMETHING VERY BAD HAS HAPPENED 1!!\n";
                    cout << "new_order_ptr->shares: " << new_order_ptr->shares << endl;
                    cout << "incomming_order->shares: " << incomming_order->shares << endl;
                    cout << "SOMETHING VERY BAD HAS HAPPENED 1!!\n";
                }

                // recursion to create match 
                this->create_match(new_order_ptr, limit_node);
            }
            if (this->logging) std::cout << "DONE traversing limit node's orders.\n" << std::endl;

            if (incomming_order->shares == 0){
                if (this->logging) std::cout << "incoming order has been completley filled :)\n";
                if (this->logging) std::cout << "deleting order: " << incomming_order->order_id << std::endl;
                this->order_map.erase(incomming_order->order_id);
                delete incomming_order;
            } else if (limit_node.head_order && limit_node.head_order->shares > incomming_order->shares){
                if (this->logging) cout << "partially filled, can still fill more.\n";
                this->create_match(incomming_order, limit_node);
            } else {
                // change limit nodes and keep trying to fill order 
                if (this->logging) std::cout << "\nchanging limit nodes and "; 
                if (incomming_order->order_type == "buy") {
                    if (this->logging) std::cout << "trying again with: " << tmp_prev->limit_price << std::endl;
                    this->create_match(incomming_order, *tmp_prev);
                } else {
                    if (this->logging) std::cout << "trying again with: " << tmp_next->limit_price << std::endl;
                    this->create_match(incomming_order, *tmp_next);
                }
            }


            if (incomming_order->shares > 0){
                if (this->logging) std::cout << "\nincoming order still has shares to buy/sell! " << std::endl;
                if (this->limit_map.find(incomming_order->limit) == this->limit_map.end()) {
                    if (this->logging) std::cout << ">> creating it a limit node: ";

                    Limit& limit_node = this->insert_limit_map(incomming_order->limit, incomming_order->shares, incomming_order->shares);
                    this->insert_limit_dll(&limit_node);
                    this->insert_order_dll(incomming_order, limit_node);
                    
                    std::cout << limit_node.limit_price << std::endl;
                } else {
                    std::cout << ">> limit node already exists ???: ";
                    auto it = limit_map.find(incomming_order->limit); 
                    Limit &limit_node = it->second;  
                    std::cout << limit_node.limit_price;
                }

                this->update_limit_spread_new();

            } else if (incomming_order->shares == 0){
                if (this->logging) std::cout << "incoming order has been fully filled!\n" << std::endl;
            } else {
                std::cout << "BAD NEW BEARS\n" << std::endl;
            }

            return 0;
        } else {
            // incomming_order->shares < limit_node.head_order->shares
            if (this->logging) std::cout << "incoming " << incomming_order->order_type << " order does not have enough shares to completely fill limit_node.head_order. create match, partially fill limit_node.head_order, update limit_node.head_order, delete incomming_order, return.\n";

            srand((unsigned) time(NULL));
            int order_id = rand();
            Order *new_order_ptr = new Order {
                order_id,                                   // order_id
                limit_node.head_order->order_type,          // order_type
                incomming_order->shares,                    // shares
                limit_node.head_order->limit,               // limit
                limit_node.head_order->entry_time,          // entry_time
                limit_node.head_order->event_time           // event_time
            };

            limit_node.head_order->shares -= incomming_order->shares;
            if (limit_node.head_order->shares < 0){
                cout << "SOMETHING VERY BAD HAS HAPPENED 2!!\n";
            }

            new_order_ptr->next = limit_node.head_order;
            limit_node.head_order = new_order_ptr;

            this->create_match(incomming_order, limit_node);

            if (this->logging) std::cout << "\n\n";
            return 0;
        }
    } else {
        if (this->logging) std::cout << "this should only happen if we are trying to match a buyer with a buyer or the limit node we have doesnt have any orders.\n";
        if (this->logging) std::cout << "  >> limit_node.head_order->order_type: " << limit_node.head_order->order_type << std::endl;
        if (this->logging) std::cout << "  >> incomming_order->order_type: " << incomming_order->order_type << std::endl;
        if (this->logging) std::cout << "  >> limit_node.head_order: " << limit_node.head_order << std::endl;
        return 0;
    }
}

int OrderBook::cancel_order(){
    return 0;
}

void OrderBook::print_list(Order *n) {
    cout << "\nPrinting list..." << endl;
    while (n != nullptr) {
        cout << n->order_id << "/" << n->limit << "/" << n->order_type << "/" << n->shares << " ";
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

    if (book.highest_buy_limit){
        std::cout << "HIGHEST BUY OFFER: " << book.highest_buy_limit->limit_price << std::endl;
    } else {
        std::cout << "NO BUY OFFERS YET: " << std::endl;
    }

    if (book.lowest_sell_limit){
        std::cout << "LOWEST SELL OFFER: " << book.lowest_sell_limit->limit_price << std::endl;
    } else {
        std::cout << "NO SELL OFFERS YET: " << std::endl;
    }

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

    cout << "\nPrinting list NEW..." << endl;
    Limit *curr = book.sorted_limit_prices_head;
    while (curr != nullptr) {
        std::string s = " ";
        Order *n = curr->head_order;
        while (n != nullptr) {

            s = s + std::to_string(n->order_id) + "/" + n->order_type + "/" + std::to_string(n->shares) + "/" + std::to_string(n->limit) + " ";
            n = n->next;
        };
        cout << curr->limit_price << ": " << s << endl;
        curr = curr->next;
    }

    return os;
}

