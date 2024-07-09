#include "order_book2.hpp"

std::string OrderBook2::add_order(
    const OrderType &order_type, 
    const std::string &user_id, 
    const int &shares, 
    const double &limit_price
){
    // O(1) if limit exists, else O(log(n))
    std::shared_ptr<Order> newOrderPtr = std::make_shared<Order>(this->generate_order_id(), user_id, shares, this->get_cur_time());
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
std::shared_ptr<Limit> OrderBook2::get_or_create_limit(Map &limit_map, Set &set, OrderType type, double limit_price){
    if (limit_map.count(limit_price)){ 
        return limit_map[limit_price]; 
    } 
    std::shared_ptr<Limit> limitPtr = std::make_shared<Limit>(limit_price, type);
    set.insert(limitPtr);
    limit_map[limit_price] = limitPtr;
    return limit_map[limit_price];
}

int OrderBook2::num_limits(OrderType type){
    auto &limit_map = (type==OrderType::bid) ? this->bid_limit_map : this->ask_limit_map;
    auto &limits    = (type==OrderType::bid) ? this->bid_limits : this->ask_limits;
    if (limit_map.size() != limits.size()){
        throw std::invalid_argument("limit_map.size() != limits.size()");
    }
    return limit_map.size();
}

int OrderBook2::num_orders(OrderType type){
    auto &orders = (type==OrderType::bid) ? this->bid_order_map : this->ask_order_map;
    return orders.size();
}

double OrderBook2::prominent_limit(OrderType type){
    auto &limits = (type==OrderType::bid) ? this->bid_limits : this->ask_limits;
    if (!limits.empty()){
        return (*limits.begin())->limit_price;
    }
    return -1;
}

std::shared_ptr<Limit> OrderBook2::prominent_limit_ptr(OrderType type){
    // std::cout << "starting prominent_limit_ptr" << std::endl;
    auto &limits = (type==OrderType::bid) ? this->bid_limits : this->ask_limits;
    if (!limits.empty()){
        return *limits.begin(); 
    }
    return nullptr;
}

std::vector<double> OrderBook2::get_limits(OrderType type, int n){
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

std::string OrderBook2::generate_order_id(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<long long> dis(1000000000, 9999999999);
    std::string order_id = std::to_string(dis(gen));
    while (this->ask_order_map.count(order_id) || this->bid_order_map.count(order_id)){
        order_id = generate_order_id();
    }
    return order_id;
}

std::string OrderBook2::get_cur_time(){
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    long long milliseconds_since_epoch = duration.count();
    return std::to_string(milliseconds_since_epoch);
}

std::vector<Match> OrderBook2::process(){
    // std::cout << "starting process" << std::endl;
    std::vector<Match> soln;
    while (this->can_match()){
        auto lowest_ask_limit  = prominent_limit_ptr(OrderType::ask);
        auto highest_bid_limit = prominent_limit_ptr(OrderType::bid);
        soln.push_back(this->create_match(lowest_ask_limit, highest_bid_limit));
    }
    return soln;
}

bool OrderBook2::can_match(){
    // std::cout << "starting can_match" << std::endl;
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

Match OrderBook2::create_match(std::shared_ptr<Limit> &ask_limit, std::shared_ptr<Limit> &bid_limit){
    // std::cout << "starting create_match" << std::endl;
    // std::cout << "num bid orders: " << this->num_orders(OrderType::bid) << std::endl;
    // std::cout << "num ask orders: " << this->num_orders(OrderType::ask) << std::endl;
    // std::cout << "ask_limit: " << ask_limit->orders.size() << std::endl;
    // std::cout << "bid_limit: " << bid_limit->orders.size() << std::endl;
    
    
    Match soln;

    auto ask_order_ptr = ask_limit->orders.front();
    ask_limit->orders.pop_front();

    auto bid_order_ptr = bid_limit->orders.front();
    bid_limit->orders.pop_front();
    
    // std::cout << "got both orders" << std::endl;

    double match_price = std::min(ask_limit->limit_price, bid_limit->limit_price);
    int matchable_shares = std::min(ask_order_ptr->shares, bid_order_ptr->shares);

    if (ask_order_ptr->shares > bid_order_ptr->shares){
        ask_order_ptr->shares -= matchable_shares;
        soln = Match(
            this->generate_order_id(),
            bid_order_ptr->order_id,
            ask_order_ptr->order_id, 
            matchable_shares,
            match_price
        );
    } else if (ask_order_ptr->shares < bid_order_ptr->shares){
        bid_order_ptr->shares -= matchable_shares;
        soln = Match(
            this->generate_order_id(),
            bid_order_ptr->order_id,
            ask_order_ptr->order_id, 
            matchable_shares,
            match_price
        );
    } else {
        soln = Match(
            this->generate_order_id(),
            bid_order_ptr->order_id,
            ask_order_ptr->order_id, 
            matchable_shares, 
            match_price
        );
    }

    this->bid_order_map.erase(bid_order_ptr->order_id);
    this->ask_order_map.erase(ask_order_ptr->order_id);

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