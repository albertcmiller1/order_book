#include "order_book2.hpp"

std::string OrderBook2::add_order(
    const OrderType &order_type, 
    const std::string &user_id, 
    const int &shares, 
    const double &limit_price
){
    std::shared_ptr<Order> newOrderPtr = std::make_shared<Order>("1234", "4321", 3, "time");;
    std::shared_ptr<Limit> limitPtr;
    if (order_type==OrderType::ask){
        limitPtr = this->get_or_create_limit(
            this->ask_limit_map, 
            this->minAskLimitHeap,
            OrderType::ask, 
            limit_price
        );
        limitPtr->orders.push_back(newOrderPtr);
        return newOrderPtr->order_id;
    }
    if (order_type==OrderType::bid){
        limitPtr = this->get_or_create_limit(
            this->bid_limit_map, 
            this->maxBidLimitHeap,
            OrderType::bid, 
            limit_price
        );
        return newOrderPtr->order_id;
    } 
    // will this crash python or reset the book?
    throw std::invalid_argument("Invalid order type");
}

template<typename Map, typename Heap>
std::shared_ptr<Limit> OrderBook2::get_or_create_limit(Map &limit_map, Heap &heap, OrderType type, double limit_price){
    if (limit_map.count(limit_price)){
        return limit_map[limit_price];  
        // return limit_map.find(limit_price)->second;  
    } 
    std::shared_ptr<Limit> limitPtr = std::make_shared<Limit>(limit_price, type);
    heap.push(limitPtr);
    limit_map[limit_price] = limitPtr;
    // return limitPtr;
    return limit_map[limit_price];
}

void OrderBook2::print_ask_limits(){
    std::cout << "min ask heap: " << std::endl;
    while (!this->minAskLimitHeap.empty()) {
        const auto& p = this->minAskLimitHeap.top();
        std::cout << "Price: " << p->limit_price << std::endl;
        this->minAskLimitHeap.pop();
    }
}

void OrderBook2::print_bid_limits(){
    std::cout << "max bid heap: " << std::endl;
    while (!this->maxBidLimitHeap.empty()) {
        const auto& p = this->maxBidLimitHeap.top();
        std::cout << "Price: " << p->limit_price << std::endl;
        while (!p->orders.empty()){
            std::cout << "Order_id: " << p->orders.front()->order_id << " ";
            p->orders.pop_front();
        }
        this->maxBidLimitHeap.pop(); 
    }
}

int main() {
    OrderBook2 ob = OrderBook2();

    ob.add_order(OrderType::bid, "albert", 3, 97);
    ob.add_order(OrderType::bid, "albert", 3, 98);
    ob.add_order(OrderType::bid, "albert", 3, 99);
    ob.add_order(OrderType::bid, "albert", 3, 99); 
    
    ob.add_order(OrderType::ask, "albert", 3, 101);
    ob.add_order(OrderType::ask, "albert", 3, 102);
    ob.add_order(OrderType::ask, "albert", 3, 103);

    ob.print_ask_limits();
    ob.print_bid_limits();

    return 0;
}