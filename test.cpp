// #include "test.hpp"
using namespace std; 

class Testing{
public: 
    bool logging {true};
    unsigned int microseconds {10000};

    void clean_up(OrderBook *book){
        // delete all order pointers 
        for (auto it = book->order_map.begin(); it != book->order_map.end(); it++){
            delete it->second;
        }
        // should i traverse the limit prices and delete? 
        book->order_map.clear();
        book->limit_map.clear();
        delete book;
    }

    int create_order(OrderBook *book, string order_type, int shares, double limit){
        int order_id = rand();

        uint64_t curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        order_id = rand();
        usleep(this->microseconds);
        curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        book->add_order(
            order_id,           // order_id
            order_type,         // order_type
            shares,             // shares
            limit,              // limit
            curr_time,          // entry_time
            curr_time           // event_time
        );

        return order_id;
    }

    bool doubles_are_same(double a, double b){
        return fabs(a - b) < 0.001;
    }

    bool test_1(){
        if (this->logging) std::cout << "<-----------------------[ test_1 starting ]------------------------------->\n";
        /* 
            prove the orderbook can add a sell order. 
        */
        OrderBook *book = new OrderBook;

        int order_id = this->create_order(book, "sell", 1, 23.47);

        if (book->highest_buy_limit){ 
            cout << "1.1 failed." << endl; return false;
        } 
        if (!book->lowest_sell_limit){ 
            cout << "1.2 failed." << endl; return false;
        } 
        if (book->lowest_sell_limit->head_order != book->lowest_sell_limit->tail_order){ 
            cout << "1.3 failed." << endl; return false;
        }
        if (book->lowest_sell_limit->head_order->order_id != order_id){ 
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

    bool test_2(){
        if (this->logging) std::cout << "<-----------------------[ test_2 starting ]------------------------------->\n";
        /* 
            prove the orderbook can add a buy order. 
        */
        OrderBook *book = new OrderBook;
        int order_id = this->create_order(book, "buy", 1, 23.46);

        if (!book->highest_buy_limit){ 
            cout << "2.1 failed." << endl; return false;
        } 
        if (book->lowest_sell_limit){ 
            cout << "2.2 failed." << endl; return false;
        } 
        if (book->highest_buy_limit->head_order != book->highest_buy_limit->tail_order){ 
            cout << "2.3 failed." << endl; return false;
        }
        if (book->highest_buy_limit->head_order->order_id != order_id){ 
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

    bool test_3(){
        if (this->logging) std::cout << "<-----------------------[ test_3 starting ]------------------------------->\n";
        /* 
            prove the orderbook can create a match when a buy order crosses the spread 
        */
        OrderBook *book = new OrderBook;
        int order_id_1 = this->create_order(book, "sell", 1, 23.47);
        int order_id_2 = this->create_order(book, "buy", 1, 23.47);

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

    bool test_4(){
        if (this->logging) std::cout << "<-----------------------[ test_4 starting ]------------------------------->\n";
        /* 
            prove the orderbook can create a match when a sell order crosses the spread 
        */
        OrderBook *book = new OrderBook;
        int order_id_1 = this->create_order(book, "buy", 1, 23.48);
        int order_id_2 = this->create_order(book, "sell", 1, 23.48);

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

    bool test_5(){
        if (this->logging) std::cout << "<-----------------------[ test_5 starting ]------------------------------->\n";
        /* 
            prove the orderbook can create a match when a big sell order comes in to match with lots of buy orders 
            note: the logic assumes most_recent_trade_price is that of the big order that came in, not that of the order it had to adjust to. 
        */
        OrderBook *book = new OrderBook;

        int order_id_1 = this->create_order(book, "buy", 1, 23.41);
        int order_id_2 = this->create_order(book, "buy", 1, 23.42); 
        int order_id_3 = this->create_order(book, "buy", 1, 23.43);
        int order_id_4 = this->create_order(book, "buy", 1, 23.44);
        // SPREAD
        int order_id_5 = this->create_order(book, "sell", 4, 23.41);

        if (book->highest_buy_limit || book->lowest_sell_limit){ 
            cout << "5.1 failed." << endl; return false;
        }
        if (book->limit_map.size() != 0){ 
            cout << "5.2 failed." << endl; return false;
        }
        if (book->order_map.size() != 0){ 
            cout << "5.3 failed." << endl; return false;
        }
        if (!doubles_are_same(book->most_recent_trade_price, 23.41)){ 
            cout << "5.4 failed." << endl; return false;
        }

        if (this->logging) std::cout << *book << std::endl;
        if (this->logging) std::cout << "\n<-----------------------[ test_5 complete ]------------------------------->\n";
        this->clean_up(book);
        return true;
    }

    bool test_6(){
        if (this->logging) std::cout << "<-----------------------[ test_6 starting ]------------------------------->\n";
        /* 
            prove the orderbook can create a match when a big buy order comes in to match with lots of sell orders 
            note: the logic assumes most_recent_trade_price is that of the big order that came in, not that of the order it had to adjust to. 
        */
        OrderBook *book = new OrderBook;

        int order_id_1 = this->create_order(book, "sell", 1, 23.45);
        int order_id_2 = this->create_order(book, "sell", 1, 23.44); 
        int order_id_3 = this->create_order(book, "sell", 1, 23.43);
        int order_id_4 = this->create_order(book, "sell", 1, 23.42);
        // SPREAD
        int order_id_5 = this->create_order(book, "buy", 4, 23.45);

        if (book->highest_buy_limit || book->lowest_sell_limit){ 
            cout << "5.1 failed." << endl; return false;
        } else if (book->limit_map.size() != 0){ 
            cout << "5.2 failed." << endl; return false;
        } else if (book->order_map.size() != 0){ 
            cout << "5.3 failed." << endl; return false;
        } else if (!doubles_are_same(book->most_recent_trade_price, 23.45)){ 
            cout << "5.4 failed." << endl; return false;
        }

        if (this->logging) std::cout << *book << std::endl;
        if (this->logging) std::cout << "\n<-----------------------[ test_6 complete ]------------------------------->\n";
        this->clean_up(book);
        return true;
    }
};
