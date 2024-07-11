#include "order_book.hpp"

std::string OrderBook::add_order(
    const OrderType &order_type, 
    const std::string &user_id, 
    const int &shares, 
    const double &limit_price
){
    std::shared_ptr<Order> newOrderPtr = std::make_shared<Order>(this->generate_order_id(), user_id, limit_price, shares, this->get_cur_time());
    std::shared_ptr<Limit> limitPtr;
    if (order_type==OrderType::ask){
        this->ask_order_map[newOrderPtr->order_id] = newOrderPtr;
        limitPtr = this->get_or_create_limit(
            this->ask_limit_map, 
            this->ask_limits,
            OrderType::ask, 
            limit_price
        );
        limitPtr->orders.push_back(newOrderPtr);
        return newOrderPtr->order_id;
    }
    if (order_type==OrderType::bid){
        this->bid_order_map[newOrderPtr->order_id] = newOrderPtr;
        limitPtr = this->get_or_create_limit(
            this->bid_limit_map, 
            this->bid_limits,
            OrderType::bid, 
            limit_price
        );
        limitPtr->orders.push_back(newOrderPtr);
        return newOrderPtr->order_id;
    } 
    throw std::invalid_argument("Order type must be instance of enum OrderType"); // will this crash python or reset the book?
}

template<typename Map, typename Set>
std::shared_ptr<Limit> OrderBook::get_or_create_limit(Map &limit_map, Set &set, OrderType type, double limit_price){
    if (limit_map.count(limit_price)){ 
        return limit_map[limit_price]; 
    } 
    std::shared_ptr<Limit> limitPtr = std::make_shared<Limit>(limit_price, type);
    set.insert(limitPtr);
    limit_map[limit_price] = limitPtr;
    return limit_map[limit_price];
}

int OrderBook::num_limits(OrderType type){
    auto &limit_map = (type==OrderType::bid) ? this->bid_limit_map : this->ask_limit_map;
    auto &limits    = (type==OrderType::bid) ? this->bid_limits : this->ask_limits;
    if (limit_map.size() != limits.size()){
        throw std::invalid_argument("limit_map.size() != limits.size()");
    }
    return limit_map.size();
}

int OrderBook::num_orders(OrderType type){
    auto &orders = (type==OrderType::bid) ? this->bid_order_map : this->ask_order_map;
    return orders.size();
}

// dont really need this
double OrderBook::prominent_limit(OrderType type){
    auto &limits = (type==OrderType::bid) ? this->bid_limits : this->ask_limits;
    if (!limits.empty()){
        return (*limits.begin())->limit_price;
    }
    return -1;
}

std::shared_ptr<Limit> OrderBook::prominent_limit_ptr(OrderType type){
    auto &limits = (type==OrderType::bid) ? this->bid_limits : this->ask_limits;
    if (!limits.empty()){
        return *limits.begin(); 
    }
    return nullptr;
}

std::vector<double> OrderBook::get_limits(OrderType type, int n){
    std::vector<double> soln;
    int num_limits = (type==OrderType::bid) ? this->num_limits(OrderType::bid) : this->num_limits(OrderType::ask);
    auto &limits   = (type==OrderType::bid) ? this->bid_limits : this->ask_limits;
    int sz = std::min(n, num_limits);
    int cnt = 0;
    for (const auto &n : limits){
        if (cnt==sz){
            return soln;
        }
        soln.push_back(n->limit_price);
        ++cnt;
    }
    return soln;
}

std::string OrderBook::generate_order_id(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<long long> dis(1000000000, 9999999999);
    std::string order_id = std::to_string(dis(gen));
    while (this->ask_order_map.count(order_id) || this->bid_order_map.count(order_id)){
        order_id = generate_order_id();
    }
    return order_id;
}

std::string OrderBook::get_cur_time(){
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    long long milliseconds_since_epoch = duration.count();
    return std::to_string(milliseconds_since_epoch);
}

std::vector<Match> OrderBook::process(){
    std::vector<Match> soln;
    while (this->can_match()){
        auto lowest_ask_limit  = prominent_limit_ptr(OrderType::ask);
        auto highest_bid_limit = prominent_limit_ptr(OrderType::bid);
        soln.push_back(this->create_match(lowest_ask_limit, highest_bid_limit));
    }
    return soln;
}

bool OrderBook::can_match(){
    double high_bid = this->prominent_limit(OrderType::bid);
    double low_ask = this->prominent_limit(OrderType::ask);
    if (std::min(high_bid, low_ask) < 0){
        return false;
    }
    double spread = low_ask - high_bid;
    if (spread <= 0){
        return true;
    }
    return false;
}

// function too big, refactor 
Match OrderBook::create_match(std::shared_ptr<Limit> &ask_limit, std::shared_ptr<Limit> &bid_limit){
    Match soln;

    auto ask_order_ptr = ask_limit->orders.front();
    ask_limit->orders.pop_front();

    auto bid_order_ptr = bid_limit->orders.front();
    bid_limit->orders.pop_front();
    
    double match_price = std::min(ask_limit->limit_price, bid_limit->limit_price);
    int matchable_shares = std::min(ask_order_ptr->shares, bid_order_ptr->shares);

    if (ask_order_ptr->shares > bid_order_ptr->shares){
        ask_order_ptr->shares -= matchable_shares;
        ask_limit->orders.push_front(ask_order_ptr);
        this->bid_order_map.erase(bid_order_ptr->order_id);
        soln = Match(
            this->generate_order_id(),
            bid_order_ptr->order_id,
            ask_order_ptr->order_id, 
            matchable_shares,
            match_price
        );
    } else if (ask_order_ptr->shares < bid_order_ptr->shares){
        bid_order_ptr->shares -= matchable_shares;
        this->ask_order_map.erase(ask_order_ptr->order_id);
        bid_limit->orders.push_front(bid_order_ptr);
        soln = Match(
            this->generate_order_id(),
            bid_order_ptr->order_id,
            ask_order_ptr->order_id, 
            matchable_shares,
            match_price
        );
    } else {
        this->bid_order_map.erase(bid_order_ptr->order_id);
        this->ask_order_map.erase(ask_order_ptr->order_id);
        soln = Match(
            this->generate_order_id(),
            bid_order_ptr->order_id,
            ask_order_ptr->order_id, 
            matchable_shares, 
            match_price
        );
    }
    if (ask_limit->orders.size()==0){
        this->ask_limit_map.erase(ask_limit->limit_price);
        this->ask_limits.erase(ask_limit);
    }
    if (bid_limit->orders.size()==0){
        this->bid_limit_map.erase(bid_limit->limit_price);
        this->bid_limits.erase(bid_limit);
    }
    return soln;
}

bool OrderBook::order_in_queue(std::string &order_id){
    return (
        this->bid_order_map.count(order_id) || this->ask_order_map.count(order_id)
    );
}

void OrderBook::cancel_order(std::string &order_id){
    if (!this->order_in_queue(order_id)){
        return;
    } else if (this->bid_order_map.count(order_id)){
        this->remove_order(
            this->bid_order_map, 
            this->bid_limit_map, 
            this->bid_limits, 
            order_id
        );
    } else if (this->ask_order_map.count(order_id)){
        this->remove_order(
            this->ask_order_map, 
            this->ask_limit_map, 
            this->ask_limits, 
            order_id
        );
    } else {
        throw std::invalid_argument("Issue trying to cancel an order.");
    }
}

template<typename OrderMap, typename LimitMap, typename Set>
void OrderBook::remove_order(OrderMap &order_map, LimitMap &limit_map, Set &limits, std::string &order_id){
    auto order_ptr = order_map[order_id];
    auto limit_ptr = limit_map[order_ptr->limit_price];

    auto it = std::find(limit_ptr->orders.begin(), limit_ptr->orders.end(), order_ptr);
    if (it != limit_ptr->orders.end()) {
        limit_ptr->orders.erase(it);
    }

    if (limit_ptr->orders.size()==0){
        limit_map.erase(order_ptr->limit_price);
        limits.erase(limit_ptr);
    }
    
    order_map.erase(order_id); 
}
