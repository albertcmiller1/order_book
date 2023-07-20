#include "order_book.hpp" 
using namespace std; 

void OrderBook::add_order(
        int order_id,
        std::string order_type,
        int shares,
        double limit_price,
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

    if (this->logging) {
        std::cout << "_________incoming order________" << std::endl;
        std::cout << "| shares: " << new_order_ptr->shares << std::endl;
        std::cout << "| order_type: " << new_order_ptr->order_type << std::endl;
        std::cout << "| limit: " << new_order_ptr->limit << std::endl;
        std::cout << "| order_id: " << new_order_ptr->order_id << std::endl << std::endl;
    }
    
    // add new order to order_map
    order_map[order_id] = new_order_ptr;

    if (this->limit_map.find(limit_price) == this->limit_map.end()) {
        // this price doesnt exist in limit_map yet. 
        // check if order crosses the spread 
        // TODO: will the create_match function take care of the issue of an incominng order that will have leftovers? 
        if (this->order_crossed_spread(new_order_ptr)){
            // Limit *best_limit_node = this->find_best_limit_node_to_match_with(new_order_ptr);
            // std::cout << "TESTING__1\n";
            // if (best_limit_node->prev) std::cout << "best_limit_node->prev->limit_price " << best_limit_node->prev->limit_price << std::endl;
            // std::cout << "best_limit_node->limit_price " << best_limit_node->limit_price << std::endl;
            // if (best_limit_node->next) std::cout << "best_limit_node->next->limit_price " << best_limit_node->next->limit_price << std::endl;
            // std::cout << "TESTING__1\n";

            // test 1 and test 2 give same answer

            double best_limit_price = find_best_limit_node_to_match_with_new(new_order_ptr->order_type, new_order_ptr->limit);

            std::cout << "TESTING__2\n";
            this->print_limits_dll(this->sorted_limit_prices_head);
            Limit *new_best_limit_node = this->limit_map[best_limit_price];
            if (new_best_limit_node->prev) std::cout << "new_best_limit_node->prev->limit_price " << new_best_limit_node->prev->limit_price << std::endl;
            std::cout << "new_best_limit_node->limit_price " << new_best_limit_node->limit_price << std::endl;
            if (new_best_limit_node->next) std::cout << "new_best_limit_node->next->limit_price " << new_best_limit_node->next->limit_price << std::endl;
            std::cout << "TESTING__2\n";

            this->create_match(new_order_ptr, new_best_limit_node);
        } else {
            Limit *new_limit_node = this->insert_limit_map(limit_price, shares, shares);
            this->insert_limit_dll(new_limit_node);
            this->insert_order_dll(new_order_ptr, new_limit_node);
        }
    } else {
        // limit already exists in limit_map, find it 
        auto it = limit_map.find(limit_price); 
        Limit *existing_limit_node = it->second;  

        // check if order crosses the spread 
        if (this->order_crossed_spread(new_order_ptr)){
            this->create_match(new_order_ptr, existing_limit_node);
        } else {
            this->insert_order_dll(new_order_ptr, existing_limit_node);
        }
    }
    this->update_limit_spread_new();
}


double OrderBook::find_best_limit_node_to_match_with_new(std::string incoming_order_type, double incoming_order_limit){
    this->print_limits_dll(this->sorted_limit_prices_head);

    if (incoming_order_type == "buy"){
        Limit *curr = this->sorted_limit_prices_tail;
        while (curr != nullptr) {
            cout << curr->limit_price << "/" << " ";
            if (curr->limit_price <= incoming_order_limit){
                cout << "FOUND: " << curr->limit_price << endl;
                return curr->limit_price;
            }
            curr = curr->prev;
        }
    } else {
        Limit *curr = this->sorted_limit_prices_head;
        while (curr != nullptr){
            cout << curr->limit_price << "/" << " ";
            if (curr->limit_price >= incoming_order_limit){
                cout << "FOUND: " << curr->limit_price << endl;
                return curr->limit_price;
            }
            curr = curr->next;
        }
    }
}

Limit* OrderBook::find_best_limit_node_to_match_with(Order *new_order_ptr){
    // TODO: BUG: needs to be within range, look at line 408 of bug.txt
    // this got stuck in an infinate loop
    if (this->logging) std::cout << "find and return the most appropriate limit for the new " << new_order_ptr->order_type << " order to match with. \n";
    this->print_limits_dll(this->sorted_limit_prices_head);
    Limit *curr = nullptr;
    if (new_order_ptr->order_type == "buy"){
        // not sure why but this seems to skip to the end???
        if (this->logging) std::cout << "start at tail and look for a sell order to match with ...\n";
        curr = this->sorted_limit_prices_tail;
        while (curr && curr->head_order->order_type == "sell"){
            if (this->logging) std::cout << "looking at curr->limit_price: " << curr->limit_price << std::endl;
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
        // this seemed to work, but cant access curr->prev for some reason? 
        if (this->logging) std::cout << "start at head and look for a buy order to match with ...\n";
        curr = this->sorted_limit_prices_head;
        while (curr && curr->head_order->order_type == "buy"){
            if (this->logging) std::cout << "looking at curr->limit_price: " << curr->limit_price << std::endl;
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

void OrderBook::update_limit_spread_new(){
    // this is very inificent 
    /*
        0:          853124830/buy/10/0.000000 
        0.48:       96063276/buy/10/0.480000 
        0.64:       1868190438/buy/10/0.640000 

        0.72:       318288679/sell/10/0.720000 
        100:        282475249/sell/10/100.000000    1622650073/sell/10/100.000000 

        _________incoming order________
        | shares: 10
        | order_type: sell
        | limit: 0.4
        | order_id: 1775260835

        new sell order has crossed the spread ...
        need to find and return the most appropriate limit for the new order to match with. 
        most appropriate limit will be: 0.48
        attempting to create a match...
        perfect match between buyer (96063276) and seller (1775260835)
        deleting old order...96063276
        deleting incoming order... 1775260835

        limit node is all out of orders! DELETING LIMIT NODE

        deleting middle
        updating limit spread
        [1]    78355 segmentation fault  ./socket

    */ 

    if (this->debug) std::cout << "updating limit spread\n";

    this->highest_buy_limit = nullptr;
    this->lowest_sell_limit = nullptr;
    Limit *curr = this->sorted_limit_prices_head;

    if (this->debug) std::cout << "starting loop\n";
    while (curr != nullptr) {
        // loop over all limits 
        if (this->logging) std::cout << "loop 1 @ node " << curr->limit_price << endl;
        if (curr->prev) if (this->logging) std::cout << "loop 1.1 @ node " << curr->prev->limit_price << endl;
        if (curr->next) if (this->logging) std::cout << "loop 1.2 @ node " << curr->next->limit_price << endl;
        Order *n = curr->head_order; // this is null for some reason 
        if (this->debug) std::cout << "loop 2\n";

        bool all_same = true;
        std::string first_order_type = n->order_type;
        if (this->debug) std::cout << "loop 3\n";
        

        while (n != nullptr) {
            // loop over limit's orders from oldest to newest 
            if (n->order_type != first_order_type){
                all_same = false;
            }
            n = n->next;
        };

        if (this->debug) std::cout << "loop 4\n";
        if (!all_same){
            std::cout << "??????AGGHH\n" << endl;
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

void OrderBook::update_limit_spread(Limit *limit, std::string order_type){
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

void OrderBook::insert_order_dll(Order *order, Limit *limit_node){
    // O(1) insertion and should also guarantee the LL is ordererd from oldest to newest
    if (limit_node->head_order) {
        Order *new_node = new Order;

        new_node->order_id = order->order_id;
        new_node->order_type = order->order_type;
        new_node->shares = order->shares;
        new_node->limit = order->limit;
        new_node->entry_time = order->entry_time;
        new_node->event_time = order->event_time;

        limit_node->tail_order->next = new_node;
        new_node->prev = limit_node->tail_order;
        new_node->next = nullptr;

        limit_node->tail_order = new_node;
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

        limit_node->head_order = first_node;
        limit_node->tail_order = first_node;
    }
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
            s = s + std::to_string(n->order_id) + "/" + n->order_type + "/" + std::to_string(n->shares) + "/" + std::to_string(n->limit) + " ";
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
    
    // compare against limit price's DLL
    // NOTE: this is only comparing against head order. i think sould be fine though. (all order of a Limit dll should have the same order type, and the DLL's head should always be the oldest order.)

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

int OrderBook::create_match(Order *incomming_order, Limit *limit_node){
    if (this->logging) std::cout << "attempting to create a match...\n";
    Limit *tmp_prev = limit_node->prev;
    Limit *tmp_next = limit_node->next;

    if (limit_node->head_order && incomming_order->order_type != limit_node->head_order->order_type){
        int buyers_order_id {0};
        int sellers_order_id {0};
        srand((unsigned) time(NULL));

        // set buyer and seller
        if (incomming_order->order_type == "buy"){
            buyers_order_id = incomming_order->order_id;
            sellers_order_id = limit_node->head_order->order_id;
        } else {
            buyers_order_id = limit_node->head_order->order_id;
            sellers_order_id = incomming_order->order_id;
        }

        if (incomming_order->shares == limit_node->head_order->shares){
            // remove old (matched with) order from DLL
            if (this->logging) std::cout << "perfect match between buyer (" << buyers_order_id << ") and seller (" << sellers_order_id << ")"  << std::endl;

            //TODO: make sure the price is the same from buyer to seller ... 

            // create a new match 
            Match new_match = Match {
                rand(),                     // match_id
                buyers_order_id,            // buying_order_id;
                sellers_order_id,           // selling_order_id;
                1,                          // sale_quantity;
                incomming_order->limit      // sale_price;
            };

            // update most_recent_trade_price
            this->most_recent_trade_price = incomming_order->limit;

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
                if (this->logging) std::cout << "\nlimit node is all out of orders! DELETING LIMIT NODE @ " << limit_node->limit_price << std::endl;
                this->print_limits_dll(this->sorted_limit_prices_head);

                if (limit_node->prev && limit_node->next){
                    if (this->logging) std::cout << "\ndeleting middle\n";
                    // middle of limit_dll
                    if (this->logging) std::cout << "limit_node->limit_price: " << limit_node->limit_price << std::endl;
                    if (this->logging) std::cout << "limit_node->prev->limit_price: " << limit_node->prev->limit_price << std::endl;
                    if (this->logging) std::cout << "limit_node->next->limit_price: " << limit_node->next->limit_price << std::endl;
                    if (this->logging) std::cout << "limit_node->prev->next->limit_price: " << limit_node->prev->next->limit_price << std::endl;
                    if (this->logging) std::cout << "limit_node->next->prev->limit_price: " << limit_node->next->prev->limit_price << std::endl;

                    limit_node->prev->next = limit_node->next;
                    limit_node->next->prev = limit_node->prev;

                } else if (limit_node->prev && !limit_node->next){
                    if (this->logging) std::cout << "\ndeleting tail\n";
                    // tail of limit_dll
                    // memory leak? 
                    limit_node->prev->next = nullptr;
                } else if (limit_node->next && !limit_node->prev){
                    if (this->logging) std::cout << "\ndeleting head\n";
                    // head of limit_dll
                    this->sorted_limit_prices_head = this->sorted_limit_prices_head->next;
                    limit_node->next->prev = nullptr;
                } else {
                    if (this->logging) std::cout << "\ndeleting only existing\n";
                    // only one limit node exists
                    this->sorted_limit_prices_head = nullptr;
                    this->sorted_limit_prices_tail = nullptr;
                }

                this->limit_map.erase(limit_node->limit_price);
                delete limit_node; 
                limit_node = nullptr;
                // delete limit_node; 
                this->print_limits_dll(this->sorted_limit_prices_head);
            }

            return 0;

        } else if (incomming_order->shares > limit_node->head_order->shares){
            if (this->logging) std::cout << "incoming order (" << incomming_order->limit <<  ") wants to " << incomming_order->order_type << " more orders than the limit_node.head_order has. create match, delete limit_node.head_order, and continue trying to fill orders \n";

            if (this->logging) std::cout << "STARTING traversing limit node (" << limit_node->limit_price << ") orders to create matches.\n" << std::endl;
            while (limit_node->head_order && incomming_order->shares >= limit_node->head_order->shares){
                // create a new order which will match the quantity of the limit_node.head_order 
                srand((unsigned) time(NULL));
                int order_id = rand();
                Order *new_order_ptr = new Order {
                    order_id,                           // order_id
                    incomming_order->order_type,        // order_type
                    limit_node->head_order->shares,      // shares
                    incomming_order->limit,             // limit
                    incomming_order->entry_time,        // entry_time
                    incomming_order->entry_time         // event_time
                };

                // add new order to order_map
                order_map[order_id] = new_order_ptr;
                incomming_order->shares -= limit_node->head_order->shares; 
                if (incomming_order->shares < 0){
                    cout << "?????? SOMETHING VERY BAD HAS HAPPENED 1!!\n";
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
            } else if (limit_node->head_order && limit_node->head_order->shares > incomming_order->shares){
                if (this->logging) cout << "partially filled, can still fill more.\n";
                this->create_match(incomming_order, limit_node);
            } else {
                // change limit nodes and keep trying to fill order 
                if (this->logging) std::cout << "\nchanging limit nodes and "; 
                if (incomming_order->order_type == "buy") {
                    if (this->logging) std::cout << "trying again with: " << tmp_prev->limit_price << std::endl;
                    this->create_match(incomming_order, tmp_prev);
                } else {
                    if (this->logging) std::cout << "trying again with: " << tmp_next->limit_price << std::endl;
                    this->create_match(incomming_order, tmp_next);
                }
            }

            if (incomming_order->shares > 0){
                if (this->logging) std::cout << "\nincoming order still has shares to buy/sell! " << std::endl;
                if (this->limit_map.find(incomming_order->limit) == this->limit_map.end()) {
                    if (this->logging) std::cout << ">> creating it a limit node: ";

                    Limit *limit_node = this->insert_limit_map(incomming_order->limit, incomming_order->shares, incomming_order->shares);
                    this->insert_limit_dll(limit_node);
                    this->insert_order_dll(incomming_order, limit_node);
                    
                    std::cout << limit_node->limit_price << std::endl;
                } else {
                    std::cout << ">> limit node already exists ???: ";
                    auto it = limit_map.find(incomming_order->limit); 
                    Limit *limit_node = it->second;  
                    std::cout << limit_node->limit_price;
                }

                this->update_limit_spread_new();

            } else if (incomming_order->shares == 0){
                if (this->logging) std::cout << "incoming order has been fully filled!\n" << std::endl;
            } else {
                std::cout << "??????? BAD NEW BEARS\n" << std::endl;
            }

            return 0;
        } else {
            // incomming_order->shares < limit_node.head_order->shares
            if (this->logging) std::cout << "incoming " << incomming_order->order_type << " order does not have enough shares to completely fill limit_node.head_order. create match, partially fill limit_node.head_order, update limit_node.head_order, delete incomming_order, return.\n";

            srand((unsigned) time(NULL));
            int order_id = rand();
            Order *new_order_ptr = new Order {
                order_id,                                    // order_id
                limit_node->head_order->order_type,          // order_type
                incomming_order->shares,                     // shares
                limit_node->head_order->limit,               // limit
                limit_node->head_order->entry_time,          // entry_time
                limit_node->head_order->event_time           // event_time
            };

            Order *order = this->order_map.at(limit_node->head_order->order_id); // TODO: why do i have to do this ??? i think we can take it out now that limits are pointers? 
            order->shares -= incomming_order->shares;
            limit_node->head_order->shares -= incomming_order->shares;
            if (limit_node->head_order->shares < 0){
                cout << "???????? SOMETHING VERY BAD HAS HAPPENED 2!!\n";
            }

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
    cout << "\nPrinting list..." << endl;
    while (n != nullptr) {
        cout << n->limit_price << "/" << " ";
        n = n->next;
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

    std::cout << "\norder_map (" << book.order_map.size() << "):" << std::endl;
    std::cout << "------------" << std::endl;
    std::cout << "order_id\t   limit        qantity\t\torder_type" << std::endl;
    for (auto it = book.order_map.begin(); it != book.order_map.end(); it++){
        os << it->first << "\t   " << it->second->limit << "\t\t" << it->second->shares << "\t\t" << it->second->order_type << std::endl;
    }

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
                s = s + std::to_string(n->order_id) + " ";
                n = n->next;
        };

        os << it->first << "\t   " << it->second->total_volume << "\t\t"  << num_orders_at_limit_price << "\t\t" << s << std::endl;
    }

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
            s = s + std::to_string(n->order_id) + "/" + n->order_type + "/" + std::to_string(n->shares) + "/" + std::to_string(n->limit) + " ";
            n = n->next;
        };
        cout << curr->limit_price << ": " << s << endl;
        curr = curr->next;
    }

    std::cout << "\nnum_orders: " << num_orders << std::endl;
    std::cout << "num_limits: " << num_limits << std::endl;
    if (true) std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~end printing book~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";
    return os;
}

