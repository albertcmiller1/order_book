#include "order_book2.hpp"

std::string OrderBook2::add_order(
    const OrderType &order_type, 
    const std::string &user_id, 
    const int &shares, 
    const double &limit_price
){
    std::shared_ptr<Order> newOrderPtr = std::make_shared<Order>(this->generate_order_id(), user_id, shares, this->get_cur_time());;
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
    auto &limits = (type==OrderType::bid) ? this->bid_limits : this->ask_limits;

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

std::vector<double> OrderBook2::get_limits(OrderType type, int n=10){
    std::vector<double> soln;
    int num_limits  = (type==OrderType::bid) ? this->num_limits(OrderType::bid) : this->num_limits(OrderType::ask);
    auto &limits    = (type==OrderType::bid) ? this->bid_limits : this->ask_limits;
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