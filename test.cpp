// #include "test.hpp"
using namespace std; 

class Testing{
public: 
    bool logging {true};
    unsigned int microseconds {10000};

    int create_order(OrderBook *book, string order_type, int shares, float limit){
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

    bool floats_are_same(double a, double b){
        cout << "A: " << a << endl;
        cout << "B: " << b << endl;
        return fabs(a - b) < 0.001;
    }


    bool test_1(OrderBook *book){
        /* 
            prove the orderbook can add a sell order. 
        */

        int order_id = this->create_order(book, "sell", 1, 23.47);

        if (book->highest_buy_limit){
            cout << "1.1 failed." << endl; return false;
        } 
        if (book->lowest_sell_limit->head_order != book->lowest_sell_limit->tail_order){
            cout << "1.2 failed." << endl; return false;
        }
        if (book->lowest_sell_limit->head_order->order_id != order_id){
            cout << "1.3 failed." << endl; return false;
        }
        if (book->most_recent_trade_price){
            cout << "1.4 failed." << endl; return false;
        }
        if (book->limit_map.size() != 1){
            cout << "1.5 failed." << endl; return false;
        }
        if (book->order_map.size() != 1){
            cout << "1.6 failed." << endl; return false;
        }

        if (this->logging) std::cout << *book << std::endl;
        if (this->logging) std::cout << "\n\n<-----------------------[ test_1 passed ]------------------------------->\n";
        return true;
    }

    bool test_2(OrderBook *book){
        /* 
            prove the orderbook can add a buy order. 
        */


        int order_id = this->create_order(book, "buy", 1, 23.46);

        if (book->highest_buy_limit->head_order != book->highest_buy_limit->tail_order){ 
            cout << "2.1 failed." << endl; return false;
        }
        if (!book->highest_buy_limit || !book->lowest_sell_limit){ 
            cout << "2.2 failed." << endl; return false;
        } 
        if (book->highest_buy_limit->head_order->order_id != order_id){ 
            cout << "2.3 failed." << endl; return false;
        }
        if (book->most_recent_trade_price){ 
            cout << "2.4 failed." << endl; return false;
        }
        if (book->limit_map.size() != 2){ 
            cout << "2.5 failed." << endl; return false;
        }
        if (book->order_map.size() != 2){ 
            cout << "2.6 failed." << endl; return false;
        }

        if (this->logging) std::cout << *book << std::endl;
        if (this->logging) std::cout << "\n\n<-----------------------[ test_2 passed ]------------------------------->\n";
        return true;
    }

    bool test_3(OrderBook *book){
        /* 
            prove the orderbook can create a match when a buy order crosses the spread 
        */

        int order_id = this->create_order(book, "buy", 1, 23.47);

        if (book->highest_buy_limit->head_order != book->highest_buy_limit->tail_order){ 
            cout << "3.1 failed." << endl; return false;
        }

        if (!book->highest_buy_limit || book->lowest_sell_limit){ 
            cout << "3.2 failed." << endl; return false;
        } 

        if (!floats_are_same(book->most_recent_trade_price, 23.47)){ 
            cout << "3.3 failed." << endl; return false;
        }

        // if (book->limit_map.size() != 2){ 
        //     cout << "2.5 failed." << endl; return false;
        // }

        // if (book->order_map.size() != 2){ 
        //     cout << "2.6 failed." << endl; return false;
        // }

 

        if (this->logging) std::cout << *book << std::endl;
        if (this->logging) std::cout << "\n\n<-----------------------[ test_3 passed ]------------------------------->\n";
        return true;
    }


};
