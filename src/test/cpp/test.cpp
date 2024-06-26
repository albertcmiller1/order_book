#include "test.hpp"
using namespace std;

std::string Testing::build_order(OrderBook *book, string order_type, int shares, double limit){
    return book->add_order(order_type, "albert", shares, limit);
}

bool Testing::doubles_are_same(double a, double b){ return fabs(a - b) < 0.001; }

void Testing::clean_up(OrderBook *book){
    book->highest_buy_limit = nullptr;
    book->lowest_sell_limit = nullptr;

    // delete all limits
    Limit *curr_limit = book->sorted_limit_prices_head;
    while (curr_limit) {
        Limit *nxt_limit  = curr_limit->next;
        Order *curr_order = curr_limit->head_order;
        
        // delete all orders attached to current limit 
        while (curr_order){
            Order *nxt_order = curr_order->next;
            delete(curr_order);
            curr_order = nullptr;
            if (nxt_order){ nxt_order->prev = nullptr; }
            curr_limit->head_order = nxt_order;
            curr_order = nxt_order;
        }

        delete(curr_limit);
        curr_limit = nullptr;
        if (nxt_limit){ nxt_limit->prev = nullptr; }
        book->sorted_limit_prices_head = nxt_limit;
        curr_limit = nxt_limit;
    }

    book->order_map.clear();
    book->limit_map.clear();
    
    delete(book);
    book = nullptr;
}


bool Testing::test_1(){
    if (this->logging) std::cout << "<-----------------------[ test_1 starting ]------------------------------->\n";
    /* 
        prove the orderbook can add a buy order 
    */
    OrderBook *book = new OrderBook;
    string order_id = this->build_order(book, "buy", 1, 23.46);

    if (!book->highest_buy_limit){ 
        cout << "1.1 failed." << endl; return false;
    } 
    if (book->lowest_sell_limit){ 
        cout << "1.2 failed." << endl; return false;
    } 
    if (book->highest_buy_limit->head_order != book->highest_buy_limit->tail_order){ 
        cout << "1.3 failed." << endl; return false;
    }
    if (book->highest_buy_limit->head_order->order_id != order_id){ 
        cout << "1.4 failed." << endl; return false;
    }
    if (book->most_recent_trade_price){ 
        cout << "1.5 failed." << endl; return false;
    }
    if (book->limit_map.size() != 1){ 
        cout << "1.6 failed." << endl; return false;
    }
    if (book->order_map.size() != 1){ 
        cout << "1.7 failed." << endl; return false;
    }

    if (this->logging) std::cout << *book << std::endl;
    if (this->logging) std::cout << "\n<-----------------------[ test_1 complete ]------------------------------->\n";
    this->clean_up(book);
    return true;
}

bool Testing::test_2(){
    if (this->logging) std::cout << "<-----------------------[ test_2 starting ]------------------------------->\n";
    /* 
        prove the orderbook can add a sell order 
    */
    OrderBook *book = new OrderBook;

    string order_id = this->build_order(book, "sell", 1, 23.47);

    if (book->highest_buy_limit){ 
        cout << "2.1 failed." << endl; return false;
    } 
    if (!book->lowest_sell_limit){ 
        cout << "2.2 failed." << endl; return false;
    } 
    if (book->lowest_sell_limit->head_order != book->lowest_sell_limit->tail_order){ 
        cout << "2.3 failed." << endl; return false;
    }
    if (book->lowest_sell_limit->head_order->order_id != order_id){ 
        cout << "2.4 failed." << endl; return false;
    }
    if (book->most_recent_trade_price){ 
        cout << "2.5 failed." << endl; return false;
    }
    if (book->limit_map.size() != 1){ 
        cout << "2.6 failed." << endl; return false;
    }
    if (book->order_map.size() != 1){ 
        cout << "2.7 failed." << endl; return false;
    }

    if (this->logging) std::cout << *book << std::endl;
    if (this->logging) std::cout << "\n<-----------------------[ test_2 complete ]------------------------------->\n";
    this->clean_up(book);
    return true;
}

bool Testing::test_3(){
    if (this->logging) std::cout << "<-----------------------[ test_3 starting ]------------------------------->\n";
    /* 
        prove a buy order can create a match with a sell order
    */
    OrderBook *book = new OrderBook;
    string order_id_1 = this->build_order(book, "sell", 1, 23.47);
    string order_id_2 = this->build_order(book, "buy", 1, 23.47);

    if (book->highest_buy_limit || book->lowest_sell_limit){ 
        cout << "3.1 failed." << endl; return false; 
    } 
    if (!doubles_are_same(book->most_recent_trade_price, 23.47)){ 
        cout << "3.2 failed." << endl; return false;
    }
    if (book->limit_map.size() != 0){ 
        cout << "3.3 failed." << endl; return false;
    }
    if (book->order_map.size() != 0){ 
        cout << "3.4 failed." << endl; return false;
    }

    if (this->logging) std::cout << *book << std::endl;
    if (this->logging) std::cout << "\n<-----------------------[ test_3 complete ]------------------------------->\n";
    this->clean_up(book);
    return true;
}

bool Testing::test_4(){
    if (this->logging) std::cout << "<-----------------------[ test_4 starting ]------------------------------->\n";
    /* 
        prove a sell order can create a match with a buy order
    */
    OrderBook *book = new OrderBook;
    string order_id_1 = this->build_order(book, "buy", 1, 23.48);
    string order_id_2 = this->build_order(book, "sell", 1, 23.48);

    if (book->highest_buy_limit || book->lowest_sell_limit){ 
        cout << "4.1 failed." << endl; return false;
    } 
    if (!doubles_are_same(book->most_recent_trade_price, 23.48)){ 
        cout << "4.2 failed." << endl; return false;
    }
    if (book->limit_map.size() != 0){ 
        cout << "4.3 failed." << endl; return false;
    }
    if (book->order_map.size() != 0){ 
        cout << "4.4 failed." << endl; return false;
    }

    if (this->logging) std::cout << *book << std::endl;
    if (this->logging) std::cout << "\n<-----------------------[ test_4 complete ]------------------------------->\n";
    this->clean_up(book);
    return true;
}

bool Testing::test_5(){
    if (this->logging) std::cout << "<-----------------------[ test_5 starting ]------------------------------->\n";
    /* 
        prove a buy order can change limit nodes to fill orders
    */
    OrderBook *book = new OrderBook;

    string order_id_1 = this->build_order(book, "sell", 1, 23.45);
    string order_id_2 = this->build_order(book, "sell", 1, 23.44); 
    string order_id_3 = this->build_order(book, "sell", 1, 23.43);
    string order_id_4 = this->build_order(book, "sell", 1, 23.42);
    // SPREAD
    string order_id_5 = this->build_order(book, "buy", 4, 23.45);

    if (book->highest_buy_limit || book->lowest_sell_limit){ 
        cout << "5.1 failed." << endl; return false;
    } 
    if (book->limit_map.size() != 0){ 
        cout << "5.2 failed." << endl; return false;
    } 
    if (book->order_map.size() != 0){ 
        cout << "5.3 failed." << endl; return false;
    } 
    if (!doubles_are_same(book->most_recent_trade_price, 23.42)){ 
        cout << "5.4 failed." << endl; return false;
    }

    if (this->logging) std::cout << *book << std::endl;
    if (this->logging) std::cout << "\n<-----------------------[ test_5 complete ]------------------------------->\n";
    this->clean_up(book);
    return true;
}

bool Testing::test_6(){
    if (this->logging) std::cout << "<-----------------------[ test_6 starting ]------------------------------->\n";
    /* 
        prove a sell order can change limit nodes to fill orders
        note: the logic assumes most_recent_trade_price is that of the big order that came in, not that of the order it had to adjust to. 
    */
    OrderBook *book = new OrderBook;

    string order_id_1 = this->build_order(book, "buy", 1, 23.41);
    string order_id_2 = this->build_order(book, "buy", 1, 23.42); 
    string order_id_3 = this->build_order(book, "buy", 1, 23.43);
    string order_id_4 = this->build_order(book, "buy", 1, 23.44);
    // SPREAD
    string order_id_5 = this->build_order(book, "sell", 4, 23.41);

    if (book->highest_buy_limit || book->lowest_sell_limit){ 
        cout << "6.1 failed." << endl; return false;
    }
    if (book->limit_map.size() != 0){ 
        cout << "6.2 failed." << endl; return false;
    }
    if (book->order_map.size() != 0){ 
        cout << "6.3 failed." << endl; return false;
    }
    if (!doubles_are_same(book->most_recent_trade_price, 23.44)){ 
        cout << "6.4 failed." << endl; return false;
    }

    if (this->logging) std::cout << *book << std::endl;
    if (this->logging) std::cout << "\n<-----------------------[ test_6 complete ]------------------------------->\n";
    this->clean_up(book);
    return true;
}

bool Testing::test_7(){
    if (this->logging) std::cout << "<-----------------------[ test_7 starting ]------------------------------->\n";
    /* 
        prove a buy order can partially fill a sell order 
    */
    OrderBook *book = new OrderBook;

    string order_id_1 = this->build_order(book, "sell", 100, 23.45);
    // SPREAD
    string order_id_2 = this->build_order(book, "buy", 50, 23.45);

    if (book->highest_buy_limit){ 
        cout << "7.1 failed." << endl; return false;
    } 
    if (!book->lowest_sell_limit){ 
        cout << "7.2 failed." << endl; return false;
    } 
    if (book->limit_map.size() != 1){ 
        cout << "7.2 failed." << endl; return false;
    } 
    if (book->order_map.size() != 1){ 
        cout << "7.3 failed." << endl; return false;
    } 
    if (!doubles_are_same(book->most_recent_trade_price, 23.45)){ 
        cout << "7.4 failed." << endl; return false;
    }
    if (book->lowest_sell_limit->head_order->shares != 50){ 
        cout << "7.5 failed." << endl; return false;
    } 
    if (book->order_map.at(book->lowest_sell_limit->head_order->order_id)->shares != 50){
        cout << "7.6 failed." << endl; return false;
    }

    if (this->logging) std::cout << *book << std::endl;
    if (this->logging) std::cout << "\n<-----------------------[ test_7 complete ]------------------------------->\n";
    this->clean_up(book); 
    return true;
}

bool Testing::test_8(){
    if (this->logging) std::cout << "<-----------------------[ test_8 starting ]------------------------------->\n";
    /* 
        prove a sell order can partially fill a buy order 
    */
    OrderBook *book = new OrderBook;

    string order_id_2 = this->build_order(book, "buy", 100, 23.46);
    // SPREAD
    string order_id_1 = this->build_order(book, "sell", 50, 23.46);

    if (book->lowest_sell_limit){ 
        cout << "8.1 failed." << endl; return false;
    } 
    if (!book->highest_buy_limit){ 
        cout << "8.2 failed." << endl; return false;
    } 
    if (book->limit_map.size() != 1){ 
        cout << "8.2 failed." << endl; return false;
    } 
    if (book->order_map.size() != 1){ 
        cout << "8.3 failed." << endl; return false;
    } 
    if (!doubles_are_same(book->most_recent_trade_price, 23.46)){ 
        cout << "8.4 failed." << endl; return false;
    }
    if (book->highest_buy_limit->head_order->shares != 50){ 
        cout << "8.5 failed." << endl; return false;
    } 
    if (book->order_map.at(book->highest_buy_limit->head_order->order_id)->shares != 50){
        cout << "8.6 failed." << endl; return false;
    }

    if (this->logging) std::cout << *book << std::endl;
    if (this->logging) std::cout << "\n<-----------------------[ test_8 complete ]------------------------------->\n";
    this->clean_up(book); 
    return true;
}

bool Testing::test_9(){
    if (this->logging) std::cout << "<-----------------------[ test_9 starting ]------------------------------->\n";
    /* 
        prove a buy order can be filled by traversing a DLL of orders of a single limit node 
    */
    OrderBook *book = new OrderBook;

    string order_id_1 = this->build_order(book, "sell", 100, 23.45);
    string order_id_2 = this->build_order(book, "sell", 100, 23.45);
    string order_id_3 = this->build_order(book, "sell", 100, 23.45);
    // SPREAD
    string order_id_4 = this->build_order(book, "buy", 200, 23.45);

    if (this->logging) std::cout << *book << std::endl;
    if (book->highest_buy_limit){ 
        cout << "9.1 failed." << endl; return false;
    } 
    if (!book->lowest_sell_limit){ 
        cout << "9.2 failed." << endl; return false;
    } 
    if (book->limit_map.size() != 1){ 
        cout << "9.2 failed." << endl; return false;
    } 
    if (book->order_map.size() != 1){ 
        cout << "9.3 failed." << endl; return false;
    } 
    if (!doubles_are_same(book->most_recent_trade_price, 23.45)){ 
        cout << "9.4 failed." << endl; return false;
    }
    if (book->lowest_sell_limit->head_order->shares != 100){ 
        cout << "9.5 failed." << endl; return false;
    } 
    if (book->order_map.at(book->lowest_sell_limit->head_order->order_id)->shares != 100){
        cout << "9.6 failed." << endl; return false;
    }

    if (this->logging) std::cout << "\n<-----------------------[ test_9 complete ]------------------------------->\n";
    this->clean_up(book); 
    return true;
}

bool Testing::test_10(){
    if (this->logging) std::cout << "<-----------------------[ test_10 starting ]------------------------------->\n";
    /* 
        prove a sell order can be filled by traversing a DLL of orders of a single limit node 
    */
    OrderBook *book = new OrderBook;

    string order_id_1 = this->build_order(book, "buy", 100, 23.47);
    string order_id_2 = this->build_order(book, "buy", 100, 23.47);
    string order_id_3 = this->build_order(book, "buy", 100, 23.47);
    // SPREAD
    string order_id_4 = this->build_order(book, "sell", 200, 23.47);

    if (!book->highest_buy_limit){ 
        cout << "10.1 failed." << endl; return false;
    } 
    if (book->lowest_sell_limit){ 
        cout << "10.2 failed." << endl; return false;
    } 
    if (book->limit_map.size() != 1){ 
        cout << "10.2 failed." << endl; return false;
    } 
    if (book->order_map.size() != 1){ 
        cout << "10.3 failed." << endl; return false;
    } 
    if (!doubles_are_same(book->most_recent_trade_price, 23.47)){ 
        cout << "10.4 failed." << endl; return false;
    }
    if (book->highest_buy_limit->head_order->shares != 100){ 
        cout << "10.5 failed." << endl; return false;
    } 
    if (book->order_map.at(book->highest_buy_limit->head_order->order_id)->shares != 100){
        cout << "10.6 failed." << endl; return false;
    }

    if (this->logging) std::cout << *book << std::endl;
    if (this->logging) std::cout << "\n<-----------------------[ test_10 complete ]------------------------------->\n";
    this->clean_up(book); 
    return true;
}

bool Testing::test_11(){
    if (this->logging) std::cout << "<-----------------------[ test_11 starting ]------------------------------->\n";
    /* 
        prove a buy order can be filled when it crosses the spread at a new, very high limit price
    */
    OrderBook *book = new OrderBook;

    string order_id_1 = this->build_order(book, "buy", 10, 4.00);
    string order_id_2 = this->build_order(book, "buy", 10, 5.00);
    string order_id_3 = this->build_order(book, "buy", 10, 6.00);
    // SPREAD
    string order_id_4 = this->build_order(book, "sell", 10, 7.00);
    // TRIGGER 
    string order_id_5 = this->build_order(book, "buy", 10, 9.00);

    if (!book->highest_buy_limit){ 
        cout << "11.1 failed." << endl; return false;
    } 
    if (book->lowest_sell_limit){ 
        cout << "11.2 failed." << endl; return false;
    } 
    if (book->limit_map.size() != 3){ 
        cout << "11.3 failed." << endl; return false;
    } 
    if (book->order_map.size() != 3){ 
        cout << "11.4 failed." << endl; return false;
    } 
    if (!doubles_are_same(book->most_recent_trade_price, 7.00)){ 
        cout << "11.5 failed." << endl; return false;
    }
    if (book->highest_buy_limit->head_order->shares != 10){ 
        cout << "11.6 failed." << endl; return false;
    } 
    if (book->order_map.at(book->highest_buy_limit->head_order->order_id)->shares != 10){
        cout << "11.7 failed." << endl; return false;
    }

    if (this->logging) std::cout << *book << std::endl;
    if (this->logging) std::cout << "\n<-----------------------[ test_11 complete ]------------------------------->\n";
    this->clean_up(book); 
    return true;
}

bool Testing::test_12(){
    if (this->logging) std::cout << "<-----------------------[ test_12 starting ]------------------------------->\n";
    /* 
        prove a sell order can be filled when it crosses the spread at a new, very low limit price
    */
    OrderBook *book = new OrderBook;

    string order_id_1 = this->build_order(book, "buy", 10, 6.00);
    // SPREAD
    string order_id_2 = this->build_order(book, "sell", 10, 7.00);
    string order_id_3 = this->build_order(book, "sell", 10, 8.00);
    string order_id_4 = this->build_order(book, "sell", 10, 9.00);
    // TRIGGER 
    string order_id_5 = this->build_order(book, "sell", 10, 5.00);

    if (book->highest_buy_limit){ 
        cout << "12.1 failed." << endl; return false;
    } 
    if (!book->lowest_sell_limit){ 
        cout << "12.2 failed." << endl; return false;
    } 
    if (book->limit_map.size() != 3){ 
        cout << "12.3 failed." << endl; return false;
    } 
    if (book->order_map.size() != 3){ 
        cout << "12.4 failed." << endl; return false;
    } 
    if (!doubles_are_same(book->most_recent_trade_price, 6.00)){ 
        cout << "12.5 failed." << endl; return false;
    }
    if (book->lowest_sell_limit->head_order->shares != 10){ 
        cout << "12.6 failed." << endl; return false;
    } 
    if (book->order_map.at(book->lowest_sell_limit->head_order->order_id)->shares != 10){
        cout << "12.7 failed." << endl; return false;
    }

    if (this->logging) std::cout << *book << std::endl;
    if (this->logging) std::cout << "\n<-----------------------[ test_12 complete ]------------------------------->\n";
    this->clean_up(book); 
    return true;
}

bool Testing::test_13(){
    if (this->logging) std::cout << "<-----------------------[ test_13 starting ]------------------------------->\n";
    /* 
        prove a buy order can be filled when it crosses the spread at a new limit price between other sell limits
    */
    OrderBook *book = new OrderBook;

    string order_id_1 = this->build_order(book, "buy", 10, 13.00);
    string order_id_2 = this->build_order(book, "buy", 10, 14.00);
    string order_id_3 = this->build_order(book, "buy", 10, 15.00);
    // SPREAD
    string order_id_4 = this->build_order(book, "sell", 10, 16.00);
    string order_id_5 = this->build_order(book, "sell", 10, 17.00);
    string order_id_6 = this->build_order(book, "sell", 10, 50.00);
    // TRIGGER 
    string order_id_7 = this->build_order(book, "buy", 10, 25.00);

    if (book->highest_buy_limit->limit_price != 15.00){ 
        cout << "13.1 failed." << endl; return false;
    } 
    if (book->lowest_sell_limit->limit_price != 16.00){ 
        cout << "13.2 failed." << endl; return false;
    } 
    if (book->limit_map.size() != 5){ 
        cout << "13.3 failed." << endl; return false;
    } 
    if (book->order_map.size() != 5){ 
        cout << "13.4 failed." << endl; return false;
    } 
    if (!doubles_are_same(book->most_recent_trade_price, 17.00)){ 
        cout << "13.5 failed." << endl; return false;
    }
    if (book->highest_buy_limit->head_order->shares != 10){ 
        cout << "13.6 failed." << endl; return false;
    } 
    if (book->order_map.at(book->highest_buy_limit->head_order->order_id)->shares != 10){
        cout << "13.7 failed." << endl; return false;
    }

    if (this->logging) std::cout << *book << std::endl;
    if (this->logging) std::cout << "\n<-----------------------[ test_13 complete ]------------------------------->\n";
    this->clean_up(book); 
    return true;
}

bool Testing::test_14(){
    if (this->logging) std::cout << "<-----------------------[ test_14 starting ]------------------------------->\n";
    /* 
        prove a sell order can be filled when it crosses the spread at a new limit price between other buy limits
    */
    OrderBook *book = new OrderBook;

    string order_id_1 = this->build_order(book, "buy", 10, 4.00);
    string order_id_2 = this->build_order(book, "buy", 10, 14.00);
    string order_id_3 = this->build_order(book, "buy", 10, 15.00);
    // SPREAD
    string order_id_4 = this->build_order(book, "sell", 10, 16.00);
    string order_id_5 = this->build_order(book, "sell", 10, 17.00);
    string order_id_6 = this->build_order(book, "sell", 10, 18.00);
    // TRIGGER 
    string order_id_7 = this->build_order(book, "sell", 10, 4.00);

    if (book->highest_buy_limit->limit_price != 15.00){ 
        cout << "14.1 failed." << endl; return false;
    } 
    if (book->lowest_sell_limit->limit_price != 16.00){ 
        cout << "14.2 failed." << endl; return false;
    } 
    if (book->limit_map.size() != 5){ 
        cout << "14.3 failed." << endl; return false;
    } 
    if (book->order_map.size() != 5){ 
        cout << "14.4 failed." << endl; return false;
    } 
    if (!doubles_are_same(book->most_recent_trade_price, 4.00)){ 
        cout << "14.5 failed." << endl; return false;
    }
    if (book->highest_buy_limit->head_order->shares != 10){ 
        cout << "14.6 failed." << endl; return false;
    } 
    if (book->order_map.at(book->highest_buy_limit->head_order->order_id)->shares != 10){
        cout << "14.7 failed." << endl; return false;
    }

    if (this->logging) std::cout << *book << std::endl;
    if (this->logging) std::cout << "\n<-----------------------[ test_14 complete ]------------------------------->\n";
    this->clean_up(book); 
    return true;
}

bool Testing::test_15(){
    if (this->logging) std::cout << "<-----------------------[ test_15 starting ]------------------------------->\n";
    /* 
        prove a buy order can be filled when it crosses the spread at a new limit price next to a DLL
    */
    OrderBook *book = new OrderBook;

    string order_id_1 = this->build_order(book, "buy", 10, 6.00);
    // SPREAD
    string order_id_2 = this->build_order(book, "sell", 10, 7.00);
    string order_id_3 = this->build_order(book, "sell", 10, 9.00);
    string order_id_4 = this->build_order(book, "sell", 10, 9.00);
    // TRIGGER 
    string order_id_5 = this->build_order(book, "buy", 10, 8.00);

    if (book->highest_buy_limit->limit_price != 6.00){ 
        cout << "15.1 failed." << endl; return false;
    } 
    if (book->lowest_sell_limit->limit_price != 9.00){ 
        cout << "15.2 failed." << endl; return false;
    } 
    if (book->limit_map.size() != 2){ 
        cout << "15.3 failed." << endl; return false;
    } 
    if (book->order_map.size() != 3){ 
        cout << "15.4 failed." << endl; return false;
    } 
    if (!doubles_are_same(book->most_recent_trade_price, 7.00)){ 
        cout << "15.5 failed." << endl; return false;
    }
    if (book->highest_buy_limit->head_order->shares != 10){ 
        cout << "15.6 failed." << endl; return false;
    } 
    if (book->order_map.at(book->highest_buy_limit->head_order->order_id)->shares != 10){
        cout << "15.7 failed." << endl; return false;
    }

    if (this->logging) std::cout << *book << std::endl;
    if (this->logging) std::cout << "\n<-----------------------[ test_15 complete ]------------------------------->\n";
    this->clean_up(book); 
    return true;
}

bool Testing::test_16(){
    if (this->logging) std::cout << "<-----------------------[ test_16 starting ]------------------------------->\n";
    /* 
        prove a sell order can be filled when it crosses the spread at a new limit price next to a DLL
    */
    OrderBook *book = new OrderBook;

    // seg fault
    string order_id_2 = this->build_order(book, "buy", 10, 2.00);
    string order_id_3 = this->build_order(book, "buy", 10, 6.00);
    string order_id_4 = this->build_order(book, "buy", 10, 6.00);
    // SPREAD
    string order_id_7 = this->build_order(book, "sell", 10, 100.00);
    // TRIGGER 
    string order_id_9 = this->build_order(book, "sell", 10, 4.00);

    if (book->highest_buy_limit->limit_price != 6.00){ 
        cout << "16.1 failed." << endl; return false;
    } 
    if (book->lowest_sell_limit->limit_price != 100.00){ 
        cout << "16.2 failed." << endl; return false;
    } 
    if (book->limit_map.size() != 3){ 
        cout << "16.3 failed." << endl; return false;
    } 
    if (book->order_map.size() != 3){ 
        cout << "16.4 failed." << endl; return false;
    } 
    if (!doubles_are_same(book->most_recent_trade_price, 6.00)){ 
        cout << "16.5 failed." << endl; return false;
    }
    if (book->highest_buy_limit->head_order->shares != 10){ 
        cout << "16.6 failed." << endl; return false;
    } 
    if (book->order_map.at(book->highest_buy_limit->head_order->order_id)->shares != 10){
        cout << "16.7 failed." << endl; return false;
    }

    if (this->logging) std::cout << *book << std::endl;
    if (this->logging) std::cout << "\n<-----------------------[ test_16 complete ]------------------------------->\n";
    this->clean_up(book); 
    return true;
}

//////////////// needs work ////////////////

bool Testing::test_17(){
    if (this->logging) std::cout << "<-----------------------[ test_17 starting ]------------------------------->\n";
    /* 
        prove a buy order can be filled when it crosses the spread at a new limit price next to a DLL
    */
    OrderBook *book = new OrderBook;

    string order_id_1 = this->build_order(book, "buy", 10, 6.00);
    // SPREAD
    string order_id_2 = this->build_order(book, "sell", 10, 7.00);
    string order_id_3 = this->build_order(book, "sell", 10, 7.00);
    string order_id_4 = this->build_order(book, "sell", 10, 9.00);
    if (this->logging) std::cout << *book << std::endl;
    // TRIGGER 
    string order_id_5 = this->build_order(book, "buy", 10, 8.00);

    // if (!book->highest_buy_limit){ 
    //     cout << "17.1 failed." << endl; return false;
    // } 
    // if (book->lowest_sell_limit){ 
    //     cout << "17.2 failed." << endl; return false;
    // } 
    // if (book->limit_map.size() != 3){ 
    //     cout << "17.3 failed." << endl; return false;
    // } 
    // if (book->order_map.size() != 3){ 
    //     cout << "17.4 failed." << endl; return false;
    // } 
    // if (!doubles_are_same(book->most_recent_trade_price, 9.00)){ 
    //     cout << "17.5 failed." << endl; return false;
    // }
    // if (book->highest_buy_limit->head_order->shares != 10){ 
    //     cout << "17.6 failed." << endl; return false;
    // } 
    // if (book->order_map.at(book->highest_buy_limit->head_order->order_id)->shares != 10){
    //     cout << "17.7 failed." << endl; return false;
    // }

    if (this->logging) std::cout << *book << std::endl;
    if (this->logging) std::cout << "\n<-----------------------[ test_17 complete ]------------------------------->\n";
    this->clean_up(book); 
    return true;
}

bool Testing::test_18(){
    if (this->logging) std::cout << "<-----------------------[ test_18 starting ]------------------------------->\n";
    /* 
        prove a sell order can be filled when it crosses the spread at a new limit price next to a DLL
    */
    OrderBook *book = new OrderBook;

    // seg fault
    string order_id_1 = this->build_order(book, "buy", 10, 1.00);
    string order_id_2 = this->build_order(book, "buy", 10, 2.00);
    string order_id_3 = this->build_order(book, "buy", 10, 2.00);
    string order_id_4 = this->build_order(book, "buy", 10, 6.00);
    // SPREAD
    string order_id_5 = this->build_order(book, "sell", 10, 100.00);
    if (this->logging) std::cout << *book << std::endl;
    // TRIGGER 
    string order_id_6 = this->build_order(book, "sell", 10, 4.00);

    // if (!book->highest_buy_limit){ 
    //     cout << "18.1 failed." << endl; return false;
    // } 
    // if (book->lowest_sell_limit){ 
    //     cout << "18.2 failed." << endl; return false;
    // } 
    // if (book->limit_map.size() != 3){ 
    //     cout << "18.3 failed." << endl; return false;
    // } 
    // if (book->order_map.size() != 3){ 
    //     cout << "18.4 failed." << endl; return false;
    // } 
    // if (!doubles_are_same(book->most_recent_trade_price, 9.00)){ 
    //     cout << "18.5 failed." << endl; return false;
    // }
    // if (book->highest_buy_limit->head_order->shares != 10){ 
    //     cout << "18.6 failed." << endl; return false;
    // } 
    // if (book->order_map.at(book->highest_buy_limit->head_order->order_id)->shares != 10){
    //     cout << "18.7 failed." << endl; return false;
    // }

    if (this->logging) std::cout << *book << std::endl;
    if (this->logging) std::cout << "\n<-----------------------[ test_18 complete ]------------------------------->\n";
    this->clean_up(book); 
    return true;
}

bool Testing::test_19(){
    if (this->logging) std::cout << "<-----------------------[ test_19 starting ]------------------------------->\n";

    OrderBook *book = new OrderBook;


    string order_id_3 = this->build_order(book, "buy", 10, 98.46);
    // SPREAD
    string order_id_4 = this->build_order(book, "sell", 10, 98.61);
    string order_id_5 = this->build_order(book, "sell", 10, 98.89);
    string order_id_6 = this->build_order(book, "sell", 10, 99.15);
    string order_id_7 = this->build_order(book, "sell", 10, 99.19);

    if (this->logging) std::cout << *book << std::endl;
    // TRIGGER 
    string order_id_16 = this->build_order(book, "buy", 10, 98.62);

    // if (!book->highest_buy_limit){ 
    //     cout << "17.1 failed." << endl; return false;
    // } 
    // if (book->lowest_sell_limit){ 
    //     cout << "17.2 failed." << endl; return false;
    // } 
    // if (book->limit_map.size() != 3){ 
    //     cout << "17.3 failed." << endl; return false;
    // } 
    // if (book->order_map.size() != 3){ 
    //     cout << "17.4 failed." << endl; return false;
    // } 
    // if (!doubles_are_same(book->most_recent_trade_price, 9.00)){ 
    //     cout << "17.5 failed." << endl; return false;
    // }
    // if (book->highest_buy_limit->head_order->shares != 10){ 
    //     cout << "17.6 failed." << endl; return false;
    // } 
    // if (book->order_map.at(book->highest_buy_limit->head_order->order_id)->shares != 10){
    //     cout << "17.7 failed." << endl; return false;
    // }

    if (this->logging) std::cout << *book << std::endl;
    if (this->logging) std::cout << "\n<-----------------------[ test_19 complete ]------------------------------->\n";
    this->clean_up(book); 
    return true;
}

bool Testing::test_20(){
    if (this->logging) std::cout << "<-----------------------[ test_20 starting ]------------------------------->\n";

    OrderBook *book = new OrderBook;

    string order_id_1 = this->build_order(book, "buy", 10, 100.00);
    string order_id_2 = this->build_order(book, "buy", 10, 100.00);
    string order_id_3 = this->build_order(book, "buy", 10, 100.00);

    string order_id_4 = this->build_order(book, "sell", 10, 100.00);
    string order_id_5 = this->build_order(book, "sell", 10, 100.01);
    string order_id_6 = this->build_order(book, "sell", 10, 100.02);

    if (this->logging) std::cout << *book << std::endl;

    // if (!book->highest_buy_limit){ 
    //     cout << "17.1 failed." << endl; return false;
    // } 
    // if (book->lowest_sell_limit){ 
    //     cout << "17.2 failed." << endl; return false;
    // } 
    // if (book->limit_map.size() != 3){ 
    //     cout << "17.3 failed." << endl; return false;
    // } 
    // if (book->order_map.size() != 3){ 
    //     cout << "17.4 failed." << endl; return false;
    // } 
    // if (!doubles_are_same(book->most_recent_trade_price, 9.00)){ 
    //     cout << "17.5 failed." << endl; return false;
    // }
    // if (book->highest_buy_limit->head_order->shares != 10){ 
    //     cout << "17.6 failed." << endl; return false;
    // } 
    // if (book->order_map.at(book->highest_buy_limit->head_order->order_id)->shares != 10){
    //     cout << "17.7 failed." << endl; return false;
    // }

    if (this->logging) std::cout << *book << std::endl;
    if (this->logging) std::cout << "\n<-----------------------[ test_20 complete ]------------------------------->\n";
    this->clean_up(book); 
    return true;
}