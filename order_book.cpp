#include "order_book.hpp" 
using namespace std; 

static std::random_device              rd;
static std::mt19937                    gen(rd());
static std::uniform_int_distribution<> dis(0, 15);
std::string OrderBook::generate_order_id() {
    std::stringstream ss;
    int i;
    
    ss << std::hex;
    for (i = 0; i < 10; i++) { ss << dis(gen); }

    std::string order_id = ss.str();
    while (this->order_map.count(order_id)){
        // order id already exists. try a new one.
        order_id = generate_order_id();
    }

    return order_id;
}


void OrderBook::add_order(string order_id, std::string order_type, int shares, double limit_price, uint64_t entry_time, uint64_t event_time){
    Order *new_order_ptr = new Order {
        order_id,
        order_type,
        shares, 
        limit_price,
        entry_time,
        event_time
    };

    if (this->logging) {
        std::cout << "_________incoming order________" << std::endl;
        std::cout << "| shares: "       << new_order_ptr->shares        << std::endl;
        std::cout << "| order_type: "   << new_order_ptr->order_type    << std::endl;
        std::cout << "| limit: "        << new_order_ptr->limit         << std::endl;
        std::cout << "| order_id: "     << new_order_ptr->order_id      << std::endl << std::endl;
    }
    
    // add new order to order_map
    order_map[order_id] = new_order_ptr;

    if (this->limit_map.count(limit_price)) {
        // limit already exists in limit_map
        Limit *existing_limit_node = limit_map.find(limit_price)->second;  

        // check if order crosses the spread 
        if (this->order_crossed_spread(new_order_ptr)){
            this->create_match(new_order_ptr, existing_limit_node);
        } else {
            this->insert_order_dll(new_order_ptr, existing_limit_node);
        }
    } else  {
        // limit doesnt exist in limit_map yet
        // check if order crosses the spread 
        if (this->order_crossed_spread(new_order_ptr)){
            Limit *best_limit_node = this->find_best_limit_node_to_match_with(new_order_ptr);
            this->create_match(new_order_ptr, best_limit_node);
        } else {
            Limit *new_limit_node = this->insert_limit_map(limit_price, shares, shares);
            this->insert_limit_dll(new_limit_node);
            this->insert_order_dll(new_order_ptr, new_limit_node);
        }
    }

    this->update_limit_spread_new();
}

Limit* OrderBook::find_best_limit_node_to_match_with(Order *new_order_ptr){
    if (this->logging) std::cout << "find and return the most appropriate limit for the new " << new_order_ptr->order_type << " order to match with. \n";
    Limit *curr = nullptr;
    if (new_order_ptr->order_type == "buy"){
        // start at tail and look for a sell order to match with
        curr = this->sorted_limit_prices_tail;
        while (curr && curr->head_order->order_type == "sell"){
            if (curr->limit_price <= new_order_ptr->limit){
                break;
            }
            curr = curr->prev;
        }
        if (!curr || curr->head_order->order_type != "sell" || curr->limit_price > new_order_ptr->limit){
            std::cout << "curr: " << curr << endl;
            std::cout << "curr->head_order->order_typ: " << curr->head_order->order_type << endl;
            std::cout << "curr->limit_price: " << curr->limit_price << endl;
            throw;
        }
    } else {
        curr = this->sorted_limit_prices_head;
        while (curr && curr->head_order->order_type == "buy"){
            if (curr->limit_price >= new_order_ptr->limit){
                break;
            }
            curr = curr->next;
        }
        if (!curr || curr->head_order->order_type != "buy" || curr->limit_price < new_order_ptr->limit){
            std::cout << "curr: " << curr << endl;
            std::cout << "curr->head_order->order_typ: " << curr->head_order->order_type << endl;
            std::cout << "curr->limit_price: " << curr->limit_price << endl;
            throw;
        }
    }
    if (this->logging) std::cout << "most appropriate limit will be: " << curr->limit_price << endl;
    return curr;
}

// TODO: instead of doing this, just update do a check each time we delete or add an order. 
void OrderBook::update_limit_spread_new(){
    // this is very slow.

    if (this->debug) std::cout << "\nupdating limit spread\n";
    this->highest_buy_limit = nullptr;
    this->lowest_sell_limit = nullptr;
    Limit *curr = this->sorted_limit_prices_head;

    // loop over all limits 
    while (curr != nullptr) {
        Order *n = curr->head_order; 
        bool all_same = true;
        std::string first_order_type = n->order_type;

        // loop over limit's orders from oldest to newest 
        while (n != nullptr) {
            if (n->order_type != first_order_type){
                all_same = false;
            }
            n = n->next;
        };

        if (!all_same){
            std::cout << "not all orders on a limit have the same order_type\n" << endl;
            throw;
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
        }
        curr = curr->next;
    }
}

void OrderBook::update_limit_spread_old(Limit *limit, std::string order_type){
    if (order_type == "buy"){
        if (this->highest_buy_limit){
            if (limit->limit_price > this->highest_buy_limit->limit_price){
                this->highest_buy_limit = limit;
            }        
        } else {
            this->highest_buy_limit = limit;
        }
    }
    if (order_type == "sell"){
        if (this->lowest_sell_limit){
            if (limit->limit_price < this->lowest_sell_limit->limit_price){
                this->lowest_sell_limit = limit;
            }        
        } else {
            this->lowest_sell_limit = limit;
        }    
    }
}

void OrderBook::insert_order_dll(Order *new_order, Limit *limit_node){
    // should guarantee the LL is ordererd from oldest to newest
    if (limit_node->head_order) {
        // insert at tail 
        limit_node->tail_order->next = new_order;
        new_order->prev = limit_node->tail_order;
        new_order->next = nullptr;
        limit_node->tail_order = new_order;
        return;
    }
    // first node on limit
    new_order->prev = nullptr;
    new_order->next = nullptr;
    limit_node->head_order = new_order;
    limit_node->tail_order = new_order;
    return;
}

Limit* OrderBook::insert_limit_map(double limit_price, int size, int total_volume){
    // this->num_limit_nodes++;
 
    Limit *new_limit_node = new Limit {
        limit_price,
        size,
        total_volume,
        1,
        nullptr, 
        nullptr, 
        nullptr, 
        nullptr
    };
    this->limit_map[limit_price] = new_limit_node;
    return limit_map[limit_price];
    // return new_limit_node;
}

int OrderBook::insert_limit_dll(Limit *new_limit){
    if (this->sorted_limit_prices_head) {
        Limit *curr = this->sorted_limit_prices_head;
        while (curr != nullptr){
            if (!curr->prev && new_limit->limit_price < curr->limit_price){
                if (this->logging) std::cout << "inserting new limit node (" << new_limit->limit_price <<") at head\n";
                new_limit->prev = nullptr;
                new_limit->next = this->sorted_limit_prices_head;
                curr->prev = new_limit;
                this->sorted_limit_prices_head = new_limit;
                return 0;
            } else if (!curr->next && new_limit->limit_price > curr->limit_price) {
                if (this->logging) std::cout << "inserting new limit node (" << new_limit->limit_price << ") at tail\n";
                new_limit->prev = this->sorted_limit_prices_tail;
                this->sorted_limit_prices_tail->next = new_limit;
                new_limit->next = nullptr;
                this->sorted_limit_prices_tail = new_limit;
                return 0;
            } else if (curr->limit_price < new_limit->limit_price && new_limit->limit_price < curr->next->limit_price){
                if (this->logging) std::cout << "inserting new limit node (" << new_limit->limit_price << ") in middle\n";
                Limit *tmp_nxt = curr->next;
                curr->next = new_limit;
                new_limit->prev = curr;
                new_limit->next = tmp_nxt;
                tmp_nxt->prev = new_limit;
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
    // loop over sorted_limit_dll
    // count all orders
    // count all limits 
    // make sure the counts match up to their maps 
    // also for each order/limit, make sure its in the map
    // loop over the maps, make sure they're in the DLL


    // go through each limit price in limit_map
    // make sure all orders attached at that price are of the same order_type. 
    // would be also good to check that there are no limit nodes with empty orders (could be a memory leak)


    cout << "\nStarting Validation..." << endl;
    Limit *curr = this->sorted_limit_prices_head;
    int num_limits = 0;
    int num_orders = 0;
    while (curr != nullptr) {
        num_limits++;
        std::string s = " ";
        Order *n = curr->head_order;
        while (n != nullptr) {
            num_orders++;
            s = s + n->order_id + "/" + n->order_type + "/" + std::to_string(n->shares) + "/" + std::to_string(n->limit) + " ";
            // s = s + std::to_string(n->order_id) + "/" + n->order_type + "/" + std::to_string(n->shares) + "/" + std::to_string(n->limit) + " ";
            n = n->next;
        };
        cout << curr->limit_price << ": " << s << endl;
        curr = curr->next;
    }

    std::cout << "\nnum_orders: " << num_orders << std::endl;
    std::cout << "num_limits: " << num_limits << std::endl;

    return 0;
}

bool OrderBook::order_crossed_spread(Order *incomming_order){
    if (!this->lowest_sell_limit && !this->highest_buy_limit){
        return false;
    } else if (this->lowest_sell_limit && !this->highest_buy_limit){
        if (incomming_order->order_type == "buy" && incomming_order->limit >= this->lowest_sell_limit->limit_price){
            if (this->logging) std::cout << "new (first and only) buy order has crossed the spread ...\n";
            return true;
        } 
        return false;
    } else if (!this->lowest_sell_limit && this->highest_buy_limit){
        if (incomming_order->order_type == "sell" && incomming_order->limit <= this->highest_buy_limit->limit_price){
            if (this->logging) std::cout << "new (first and only) sell order has crossed the spread ...\n";
            return true;
        }
        return false;
    } else if (incomming_order->order_type == "buy" && incomming_order->limit >= this->lowest_sell_limit->limit_price) {
        if (this->logging) std::cout << "new buy order has crossed the spread ...\n";
        return true;
    } else if (incomming_order->order_type == "sell" && incomming_order->limit <= this->highest_buy_limit->limit_price){
        if (this->logging) std::cout << "new sell order has crossed the spread ...\n";
        return true;
    } else {
        return false;
    }
}

// TODO: this function is too big. separate each case into 3 separate functions for clarity. 
int OrderBook::create_match(Order *incomming_order, Limit *limit_node){
    if (this->logging) std::cout << "attempting to create a match...\n";
    Limit *tmp_prev = limit_node->prev;
    Limit *tmp_next = limit_node->next;

    if (limit_node->head_order && incomming_order->order_type != limit_node->head_order->order_type){
        string buyers_order_id;
        string sellers_order_id;

        // set buyer and seller
        if (incomming_order->order_type == "buy"){
            buyers_order_id = incomming_order->order_id;
            sellers_order_id = limit_node->head_order->order_id;
        } else {
            buyers_order_id = limit_node->head_order->order_id;
            sellers_order_id = incomming_order->order_id;
        }

        if (incomming_order->shares == limit_node->head_order->shares){
            if (this->logging) std::cout << "perfect match between buyer (" << buyers_order_id << ") and seller (" << sellers_order_id << ")"  << std::endl;

            //TODO: if the price isn't equal from buyer to seller, who gets the leftover money? 

            // create a new match 
            Match new_match = Match {
                rand(),                     // match_id
                buyers_order_id,            // buying_order_id;
                sellers_order_id,           // selling_order_id;
                1,                          // sale_quantity;
                incomming_order->limit      // sale_price;
            };
            
            // update most_recent_trade_price
            // this->most_recent_trade_price = incomming_order->limit;
            this->most_recent_trade_price = limit_node->limit_price; 

            // When a match is created, we will need to send a message about it to both users 

            // for (auto user : users){
            //     // user->send_text("MATCH CREATED!");
            //     std::string s = "most recent trade price: " + to_string(this->most_recent_trade_price);
            //     user->send_text(s);
            // }

            // delete head 
            if (this->logging) std::cout << "deleting old order..." << limit_node->head_order->order_id << std::endl;
            Order *tmp = limit_node->head_order; 
            limit_node->head_order = limit_node->head_order->next;
            if (limit_node->head_order) { 
                limit_node->head_order->prev=nullptr; 
            }
            this->order_map.erase(tmp->order_id);
            delete tmp;
            tmp = nullptr;

            // delete incomming_order.
            if (this->logging) std::cout << "deleting incoming order... " << incomming_order->order_id << std::endl;
            this->order_map.erase(incomming_order->order_id);
            delete incomming_order;
            incomming_order = nullptr;

            if (!limit_node->head_order){
                // if limit has no orders, delete it
                // TODO: if original incoming order still has shares to buy/sell, dont delete.
                if (this->logging) std::cout << "\nlimit node is all out of orders! DELETING LIMIT NODE @ " << limit_node->limit_price << std::endl;
                if (limit_node->prev && limit_node->next){
                    if (this->logging) std::cout << "\ndeleting middle\n";
                    limit_node->prev->next = limit_node->next;
                    limit_node->next->prev = limit_node->prev;
                } else if (limit_node->prev && !limit_node->next){
                    if (this->logging) std::cout << "\ndeleting tail\n";
                    this->sorted_limit_prices_tail = limit_node->prev;
                    limit_node->prev->next = nullptr;
                } else if (limit_node->next && !limit_node->prev){
                    if (this->logging) std::cout << "\ndeleting head\n";
                    this->sorted_limit_prices_head = this->sorted_limit_prices_head->next;
                    limit_node->next->prev = nullptr;
                } else {
                    if (this->logging) std::cout << "\ndeleting only existing\n";
                    this->sorted_limit_prices_head = nullptr;
                    this->sorted_limit_prices_tail = nullptr;
                }
                this->limit_map.erase(limit_node->limit_price);
                delete limit_node; 
                limit_node = nullptr;
            }

            return 0;

        } else if (incomming_order->shares > limit_node->head_order->shares){
            if (this->logging) std::cout << "incoming order (" << incomming_order->order_id <<  ") wants to " << incomming_order->order_type << " more orders than the limit_node->head_order has.\n";
            if (this->logging) std::cout << "starting to traverse limit node (" << limit_node->limit_price << ") orders to create matches.\n" << std::endl;
            while (limit_node->head_order && incomming_order->shares >= limit_node->head_order->shares){
                // create a new order (branched from incoming_order) which will match the quantity of the limit_node->head_order 
                // use this new order to create a perfect match 
                // OG incoming_order will still have some leftover shares

                Order *new_order_ptr = new Order {
                    generate_order_id(),                           
                    incomming_order->order_type,        
                    limit_node->head_order->shares,     
                    incomming_order->limit,             
                    incomming_order->entry_time,        
                    incomming_order->entry_time         
                };

                // add new order to order_map
                order_map[new_order_ptr->order_id] = new_order_ptr;

                // update incoming order 
                incomming_order->shares -= limit_node->head_order->shares; 
                if (incomming_order->shares < 0){ cout << "incoming order has negative shares... exiting.\n"; throw; }

                // recursion to create match 
                this->create_match(new_order_ptr, limit_node);
            }

            if (this->logging) std::cout << "DONE traversing limit node's orders.\n" << std::endl;

            if (incomming_order->shares == 0){
                if (this->logging) std::cout << "incoming_order has been completley filled :)\n";
                if (this->logging) std::cout << "deleting incoming_order: " << incomming_order->order_id << std::endl;
                this->order_map.erase(incomming_order->order_id);
                delete incomming_order;
                return 0;
            }  

            if (limit_node->head_order && limit_node->head_order->shares > incomming_order->shares){
                if (this->logging) cout << "partially filled, can still fill more.\n";
                this->create_match(incomming_order, limit_node);
            }  
            
            if (incomming_order->shares > 0) {
                // change limit nodes and keep trying to fill order 
                if (this->logging) std::cout << "\nincoming order still has " << incomming_order->shares << " to deal with.\n";
                
                if (tmp_prev && incomming_order->order_type == "buy" && tmp_prev->head_order->order_type == "sell") {
                    if (this->logging) std::cout << "changing limit nodes and trying again with: " << tmp_prev->limit_price << std::endl;
                    this->create_match(incomming_order, tmp_prev);
                } 
                
                if (tmp_next && incomming_order->order_type == "sell" && tmp_next->head_order->order_type == "buy") {
                    if (this->logging) std::cout << "changing limit nodes and trying again with: " << tmp_next->limit_price << std::endl;
                    this->create_match(incomming_order, tmp_next);
                }
            }
            
            // TODO: can clean this up a bit ^ above is under the same condition as below 
            if (incomming_order->shares > 0){
                if (this->logging) std::cout << "\nincoming order still has shares to buy/sell that cant be matched with." << std::endl;
                if (this->limit_map.count(incomming_order->limit)){
                    Limit *found_limit_node = limit_map.find(incomming_order->limit)->second;  
                    if (this->logging) std::cout << ">> limit node " << found_limit_node->limit_price << " already exists... it must have been priviously created during recursion.: ";
                } else {
                    if (this->logging) std::cout << ">> creating it a limit node: \n";
                    Limit *new_limit_node = this->insert_limit_map(incomming_order->limit, incomming_order->shares, incomming_order->shares);
                    this->insert_limit_dll(new_limit_node);
                    this->insert_order_dll(incomming_order, new_limit_node);
                }

            } else if (incomming_order->shares == 0){
                if (this->logging) std::cout << "incoming order has been fully filled!\n" << std::endl;
            } else {
                std::cout << "???\n" << std::endl; throw;
            }

            return 0;
        } else {
            // incomming_order->shares < limit_node.head_order->shares
            if (this->logging) std::cout << "incoming " << incomming_order->order_type << " order does not have enough shares to completely fill limit_node->head_order.\n";

            // create a new order branched off of limit_node.head order and place it at the the head of limit node
            Order *new_order_ptr = new Order {
                generate_order_id(),                         // order_id
                limit_node->head_order->order_type,          // order_type
                incomming_order->shares,                     // shares
                limit_node->head_order->limit,               // limit
                limit_node->head_order->entry_time,          // entry_time
                limit_node->head_order->event_time           // event_time
            };

            // update head order of limit node
            limit_node->head_order->shares -= incomming_order->shares;  
            if (limit_node->head_order->shares < 0 || incomming_order->shares < 0){ cout << "limit_node->head_order->shares is negative... exiting.\n"; throw; }

            new_order_ptr->next = limit_node->head_order;
            limit_node->head_order = new_order_ptr;
            this->create_match(incomming_order, limit_node);

            if (this->logging) std::cout << "\n\n";
            return 0;
        }
    } else {
        if (this->logging) std::cout << "this should only happen if we are trying to match a buyer with a buyer or the limit node we have doesnt have any orders.\n";
        if (this->logging) std::cout << "  >> incomming_order->order_type: " << incomming_order->order_type << std::endl;
        if (this->logging) std::cout << "  >> limit_node.head_order->order_type: " << limit_node->head_order->order_type << std::endl;
        if (this->logging) std::cout << "  >> limit_node.head_order: " << limit_node->head_order << std::endl;
        throw;
        return 0;
    }
}

int OrderBook::cancel_order(){
    return 0;
}

void OrderBook::print_orders_dll(Order *n) {
    cout << "\nPrinting list..." << endl;
    while (n != nullptr) {
        cout << n->order_id << "/" << n->limit << "/" << n->order_type << "/" << n->shares << " ";
        n = n->next;
    }
    cout << "done...\n\n" << endl;
} 

void OrderBook::print_limits_dll(Limit *n) {
    cout << "\nPrinting list forwards..." << endl;
    while (n != nullptr) {
        cout << n->limit_price << "/" << " ";
        n = n->next;
    }
    cout << "done...\n\n" << endl;
} 

void OrderBook::print_limits_dll_backwards(Limit *n) {
    cout << "\nPrinting list backwards..." << endl;
    while (n != nullptr) {
        cout << n->limit_price << "/" << " ";
        n = n->prev;
    }
    cout << "done...\n\n" << endl;
} 

std::ostream& operator<<(std::ostream& os, const OrderBook &book){
    if (true) std::cout << "\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~start printing book~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

    if (book.highest_buy_limit){
        std::cout << "HIGHEST BUY OFFER: " << book.highest_buy_limit->limit_price << std::endl;
    } else {
        std::cout << "NO BUY OFFERS YET " << std::endl;
    }

    if (book.lowest_sell_limit){
        std::cout << "LOWEST SELL OFFER: " << book.lowest_sell_limit->limit_price << std::endl;
    } else {
        std::cout << "NO SELL OFFERS YET " << std::endl;
    }

    if (book.most_recent_trade_price){
        std::cout << "MOST RECENT TRADE PRICE: " << book.most_recent_trade_price << std::endl;
    } else {
        std::cout << "NO TRADES YET " << std::endl;
    }

    if (book.limit_map.empty()){
        os << "LIMIT MAP EMPTY\n";
    }

    if (book.order_map.empty()){
        os << "ORDER MAP EMPTY";
        return os;
    }

    if (book.debug){
        std::cout << "\norder_map (" << book.order_map.size() << "):" << std::endl;
        std::cout << "------------" << std::endl;
        std::cout << "order_id\t   limit        qantity\t\torder_type" << std::endl;
        for (auto it = book.order_map.begin(); it != book.order_map.end(); it++){
            os << it->first << "\t   " << it->second->limit << "\t\t" << it->second->shares << "\t\t" << it->second->order_type << std::endl;
        }
    }

    if (book.debug){
        std::cout << "\n\nlimit_map (" << book.limit_map.size() << "):" << std::endl;
        std::cout << "------------" << std::endl;
        std::cout << "price      volume       num_orders \t order_ids" << std::endl;
        for (auto it = book.limit_map.begin(); it != book.limit_map.end(); it++){
            // this is O(n) -- instead, just keep a new field on Limit.num_orders
            int num_orders_at_limit_price {0};
            Order *curr = it->second->head_order;;
            while (curr != nullptr) {
                num_orders_at_limit_price++;
                curr = curr->next;
            };

            std::string s = " ";
            Order *n = it->second->head_order;
            while (n != nullptr) {
                    s = s + n->order_id + " ";
                    // s = s + std::to_string(n->order_id) + " ";
                    n = n->next;
            };

            os << it->first << "\t   " << it->second->total_volume << "\t\t"  << num_orders_at_limit_price << "\t\t" << s << std::endl;
        }
    }

    if (book.debug){
        cout << "\nPrinting all orders..." << endl;
        Limit *curr = book.sorted_limit_prices_head;
        int num_limits = 0;
        int num_orders = 0;
        while (curr != nullptr) {
            num_limits++;
            std::string s = " ";
            Order *n = curr->head_order;
            while (n != nullptr) {
                num_orders++;
                s = s + n->order_id + "/" + n->order_type + "/" + std::to_string(n->shares) + "/" + std::to_string(n->limit) + " ";
                // s = s + std::to_string(n->order_id) + "/" + n->order_type + "/" + std::to_string(n->shares) + "/" + std::to_string(n->limit) + " ";
                n = n->next;
            };
            cout << curr->limit_price << ": " << s << endl;
            curr = curr->next;
        }
    
        std::cout << "\nnum_orders: " << num_orders << std::endl;
        std::cout << "num_limits: " << num_limits << std::endl;
    
        if (num_orders != book.order_map.size()){
            std::cout << "num_oders counted: " << num_orders << std::endl;
            std::cout << "book.order_map.size: " << book.order_map.size() << std::endl;
            throw;
        }
    }

    int num_limits_cnt_forwards = 0;
    int num_limits_cnt_backwards = 0;

    std::cout << "\nPrinting list forwards..." << endl;
    Limit *n = book.sorted_limit_prices_head;
    while (n != nullptr) {
        num_limits_cnt_forwards++;
        std::cout << n->limit_price << "/" << " ";
        n = n->next;
    }
    std::cout << "done...\n\n" << endl;

    std::cout << "\nPrinting list backwards..." << endl;
    Limit *p = book.sorted_limit_prices_tail;

    while (p != nullptr) {
        num_limits_cnt_backwards++;
        cout << p->limit_price << "/" << " ";
        p = p->prev;
    }
    std::cout << "done...\n\n" << endl;

    if (num_limits_cnt_forwards != num_limits_cnt_backwards){
        std::cout << "num_limits_cnt_forwards: " << num_limits_cnt_forwards << std::endl;
        std::cout << "num_limits_cnt_backwards: " << num_limits_cnt_backwards << std::endl;
        throw;
    }

    if (true) std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~end printing book~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";
    return os;
}

