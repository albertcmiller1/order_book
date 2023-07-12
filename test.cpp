// #include "test.hpp"
using namespace std; 

class Testing{
public: 
    unsigned int microseconds {10000};
    bool logging = false;

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

    int count_digit(int number) {
        int count = 0;
        while(number != 0) {
            number = number / 10;
            count++;
        }
        return count;
    }

    int create_order(OrderBook *book, string order_type, int shares, double limit){
        int order_id = rand();
        
        while (count_digit(order_id) <= 9){ 
            order_id *= 10;
        }

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
            prove the orderbook can add a buy order 
        */
        OrderBook *book = new OrderBook;
        int order_id = this->create_order(book, "buy", 1, 23.46);

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

    bool test_2(){
        if (this->logging) std::cout << "<-----------------------[ test_2 starting ]------------------------------->\n";
        /* 
            prove the orderbook can add a sell order 
        */
        OrderBook *book = new OrderBook;

        int order_id = this->create_order(book, "sell", 1, 23.47);

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

    bool test_3(){
        if (this->logging) std::cout << "<-----------------------[ test_3 starting ]------------------------------->\n";
        /* 
            prove a buy order can create a match with a sell order
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
            prove a sell order can create a match with a buy order
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
            prove a buy order can change limit nodes to fill orders
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
        } 
        if (book->limit_map.size() != 0){ 
            cout << "5.2 failed." << endl; return false;
        } 
        if (book->order_map.size() != 0){ 
            cout << "5.3 failed." << endl; return false;
        } 
        if (!doubles_are_same(book->most_recent_trade_price, 23.45)){ 
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
            prove a sell order can change limit nodes to fill orders
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
            cout << "6.1 failed." << endl; return false;
        }
        if (book->limit_map.size() != 0){ 
            cout << "6.2 failed." << endl; return false;
        }
        if (book->order_map.size() != 0){ 
            cout << "6.3 failed." << endl; return false;
        }
        if (!doubles_are_same(book->most_recent_trade_price, 23.41)){ 
            cout << "6.4 failed." << endl; return false;
        }

        if (this->logging) std::cout << *book << std::endl;
        if (this->logging) std::cout << "\n<-----------------------[ test_6 complete ]------------------------------->\n";
        this->clean_up(book);
        return true;
    }

    bool test_7(){
        if (this->logging) std::cout << "<-----------------------[ test_7 starting ]------------------------------->\n";
        /* 
            prove a buy order can partially fill a sell order 
        */
        OrderBook *book = new OrderBook;

        int order_id_1 = this->create_order(book, "sell", 100, 23.45);
        // SPREAD
        int order_id_2 = this->create_order(book, "buy", 50, 23.45);

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

    bool test_8(){
        if (this->logging) std::cout << "<-----------------------[ test_8 starting ]------------------------------->\n";
        /* 
            prove a sell order can partially fill a buy order 
        */
        OrderBook *book = new OrderBook;

        int order_id_2 = this->create_order(book, "buy", 100, 23.46);
        // SPREAD
        int order_id_1 = this->create_order(book, "sell", 50, 23.46);

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

    bool test_9(){
        if (this->logging) std::cout << "<-----------------------[ test_9 starting ]------------------------------->\n";
        /* 
            prove a buy order can be filled by traversing a DLL of orders of a single limit node 
        */
        OrderBook *book = new OrderBook;

        int order_id_1 = this->create_order(book, "sell", 100, 23.45);
        int order_id_2 = this->create_order(book, "sell", 100, 23.45);
        int order_id_3 = this->create_order(book, "sell", 100, 23.45);
        // SPREAD
        int order_id_4 = this->create_order(book, "buy", 200, 23.45);

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

        if (this->logging) std::cout << *book << std::endl;
        if (this->logging) std::cout << "\n<-----------------------[ test_9 complete ]------------------------------->\n";
        this->clean_up(book); 
        return true;
    }

    bool test_10(){
        if (this->logging) std::cout << "<-----------------------[ test_10 starting ]------------------------------->\n";
        /* 
            prove a sell order can be filled by traversing a DLL of orders of a single limit node 
        */
        OrderBook *book = new OrderBook;

        int order_id_1 = this->create_order(book, "buy", 100, 23.47);
        int order_id_2 = this->create_order(book, "buy", 100, 23.47);
        int order_id_3 = this->create_order(book, "buy", 100, 23.47);
        // SPREAD
        int order_id_4 = this->create_order(book, "sell", 200, 23.47);

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

    bool test_11(){
        if (this->logging) std::cout << "<-----------------------[ test_11 starting ]------------------------------->\n";
        /* 
            prove a buy order can be filled when it crosses the spread at a new, very high limit price
        */
        OrderBook *book = new OrderBook;

        int order_id_1 = this->create_order(book, "buy", 10, 4.00);
        int order_id_2 = this->create_order(book, "buy", 10, 5.00);
        int order_id_3 = this->create_order(book, "buy", 10, 6.00);
        // SPREAD
        int order_id_4 = this->create_order(book, "sell", 10, 7.00);
        if (this->logging) std::cout << *book << std::endl;
        // TRIGGER 
        int order_id_5 = this->create_order(book, "buy", 10, 9.00);

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
        if (!doubles_are_same(book->most_recent_trade_price, 9.00)){ 
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

    bool test_12(){
        if (this->logging) std::cout << "<-----------------------[ test_12 starting ]------------------------------->\n";
        /* 
            prove a sell order can be filled when it crosses the spread at a new, very low limit price
        */
        OrderBook *book = new OrderBook;

        int order_id_1 = this->create_order(book, "buy", 10, 6.00);
        // SPREAD
        int order_id_2 = this->create_order(book, "sell", 10, 7.00);
        int order_id_3 = this->create_order(book, "sell", 10, 8.00);
        int order_id_4 = this->create_order(book, "sell", 10, 9.00);
        // TRIGGER 
        if (this->logging) std::cout << *book << std::endl;
        int order_id_5 = this->create_order(book, "sell", 10, 5.00);

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
        if (!doubles_are_same(book->most_recent_trade_price, 5.00)){ 
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

    bool test_13(){
        if (this->logging) std::cout << "<-----------------------[ test_13 starting ]------------------------------->\n";
        /* 
            prove a buy order can be filled when it crosses the spread at a new limit price between other sell limits
        */
        OrderBook *book = new OrderBook;

        // no seg fault, but there is a bug! 
        int order_id_1 = this->create_order(book, "buy", 10, 13.00);
        int order_id_2 = this->create_order(book, "buy", 10, 14.00);
        int order_id_3 = this->create_order(book, "buy", 10, 15.00);
        // SPREAD
        int order_id_4 = this->create_order(book, "sell", 10, 16.00);
        int order_id_5 = this->create_order(book, "sell", 10, 17.00);
        int order_id_6 = this->create_order(book, "sell", 10, 50.00);
        
        if (this->logging) std::cout << *book << std::endl;
        // TRIGGER 
        int order_id_7 = this->create_order(book, "buy", 10, 25.00);

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
        if (!doubles_are_same(book->most_recent_trade_price, 25.00)){ 
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

    bool test_14(){
        if (this->logging) std::cout << "<-----------------------[ test_14 starting ]------------------------------->\n";
        /* 
            prove a sell order can be filled when it crosses the spread at a new limit price between other buy limits
        */
        OrderBook *book = new OrderBook;

        // no seg fault, but there is a bug! 
        int order_id_1 = this->create_order(book, "buy", 10, 4.00);
        int order_id_2 = this->create_order(book, "buy", 10, 14.00);
        int order_id_3 = this->create_order(book, "buy", 10, 15.00);
        // SPREAD
        int order_id_4 = this->create_order(book, "sell", 10, 16.00);
        int order_id_5 = this->create_order(book, "sell", 10, 17.00);
        int order_id_6 = this->create_order(book, "sell", 10, 18.00);

        if (this->logging) std::cout << *book << std::endl;
        // TRIGGER 
        int order_id_7 = this->create_order(book, "sell", 10, 9.00);

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
        if (!doubles_are_same(book->most_recent_trade_price, 9.00)){ 
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

    bool test_15(){
        if (this->logging) std::cout << "<-----------------------[ test_15 starting ]------------------------------->\n";
        /* 
            prove a buy order can be filled when it crosses the spread at a new limit price next to a DLL
        */
        OrderBook *book = new OrderBook;

        // seg fault
        int order_id_1 = this->create_order(book, "buy", 10, 6.00);
        // SPREAD
        int order_id_2 = this->create_order(book, "sell", 10, 7.00);
        int order_id_3 = this->create_order(book, "sell", 10, 9.00);
        int order_id_4 = this->create_order(book, "sell", 10, 9.00);
        
        if (this->logging) std::cout << *book << std::endl;
        // TRIGGER 
        int order_id_5 = this->create_order(book, "buy", 10, 8.00);

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
        if (!doubles_are_same(book->most_recent_trade_price, 8.00)){ 
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

    bool test_16(){
        if (this->logging) std::cout << "<-----------------------[ test_16 starting ]------------------------------->\n";
        /* 
            prove a sell order can be filled when it crosses the spread at a new limit price next to a DLL
        */
        OrderBook *book = new OrderBook;

        // seg fault
        int order_id_2 = this->create_order(book, "buy", 10, 2.00);
        int order_id_3 = this->create_order(book, "buy", 10, 6.00);
        int order_id_4 = this->create_order(book, "buy", 10, 6.00);
        // SPREAD
        int order_id_7 = this->create_order(book, "sell", 10, 100.00);
        if (this->logging) std::cout << *book << std::endl;
        // TRIGGER 
        int order_id_9 = this->create_order(book, "sell", 10, 4.00);

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
        if (!doubles_are_same(book->most_recent_trade_price, 4.00)){ 
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

    bool test_17(){
        if (this->logging) std::cout << "<-----------------------[ test_17 starting ]------------------------------->\n";
        /* 
            prove a buy order can be filled when it crosses the spread at a new limit price next to a DLL
        */
        OrderBook *book = new OrderBook;

        int order_id_1 = this->create_order(book, "buy", 10, 6.00);
        // SPREAD
        int order_id_2 = this->create_order(book, "sell", 10, 7.00);
        int order_id_3 = this->create_order(book, "sell", 10, 7.00);
        int order_id_4 = this->create_order(book, "sell", 10, 9.00);
        if (this->logging) std::cout << *book << std::endl;
        // TRIGGER 
        int order_id_5 = this->create_order(book, "buy", 10, 8.00);

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

    bool test_18(){
        if (this->logging) std::cout << "<-----------------------[ test_18 starting ]------------------------------->\n";
        /* 
            prove a sell order can be filled when it crosses the spread at a new limit price next to a DLL
        */
        OrderBook *book = new OrderBook;

        // seg fault
        int order_id_1 = this->create_order(book, "buy", 10, 1.00);
        int order_id_2 = this->create_order(book, "buy", 10, 2.00);
        int order_id_3 = this->create_order(book, "buy", 10, 2.00);
        int order_id_4 = this->create_order(book, "buy", 10, 6.00);
        // SPREAD
        int order_id_5 = this->create_order(book, "sell", 10, 100.00);
        if (this->logging) std::cout << *book << std::endl;
        // TRIGGER 
        int order_id_6 = this->create_order(book, "sell", 10, 4.00);

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

    bool test_19(){
        if (this->logging) std::cout << "<-----------------------[ test_19 starting ]------------------------------->\n";
        /* 
            prove a buy order can be filled when it crosses the spread at a new limit price next to a DLL

            Printing all orders...
            98.46:  1995225539/buy/10/98.460000 
            98.61:  -1755771824/sell/10/98.610000 
            98.89:  1096154504/sell/10/98.890000 
            99.15:  1458968630/sell/10/99.150000 
            99.19:  1005380434/sell/10/99.190000 
            99.39:  1027653254/sell/10/99.390000 
            99.49:  1260673600/sell/10/99.490000 
            99.53:  1348852004/sell/10/99.530000 
            99.54:  -1964550046/sell/10/99.540000 
            99.59:  1318287110/sell/10/99.590000 
            99.69:  1589505720/sell/10/99.690000 1553983184/sell/10/99.690000 
            99.73:  1203590488/sell/10/99.730000 
            100:  -1470214806/sell/10/100.000000 
            100.27:  1395243578/sell/10/100.270000 
            100.4:  1107414540/sell/10/100.400000 
            100.42:  1074852642/sell/10/100.420000 
            100.76:  1510779076/sell/10/100.760000 
            100.94:  1869583594/sell/10/100.940000 

            num_orders: 19
            num_limits: 18
            ~~~~~~~~~~~~~~~~~~~~~~~~~~end printing book~~~~~~~~~~~~~~~~~~~~~~~~~~


            ----------------------------------------------------

            _________incoming order________
            | shares: 10
            | order_type: buy
            | limit: 98.62
            | order_id: 1596860723

            new buy order has crossed the spread ...
            find and return the most appropriate limit for the new order to match with. 
            curr: 0x600002000100
            curr->head_order->order_typ: buy
            curr->limit_price: 98.46
            libc++abi: terminating
            [1]    84766 abort      ./socket

        */
        OrderBook *book = new OrderBook;


        int order_id_3 = this->create_order(book, "buy", 10, 98.46);
        // SPREAD
        int order_id_4 = this->create_order(book, "sell", 10, 98.61);
        int order_id_5 = this->create_order(book, "sell", 10, 98.89);
        int order_id_6 = this->create_order(book, "sell", 10, 99.15);
        int order_id_7 = this->create_order(book, "sell", 10, 99.19);

        if (this->logging) std::cout << *book << std::endl;
        // TRIGGER 
        int order_id_16 = this->create_order(book, "buy", 10, 98.62);

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

};

// this caused a seg fault 
/*
HIGHEST BUY OFFER: 0.0288
LOWEST SELL OFFER: 0.06

order_map: 
------------
order_id	   limit        qantity		order_type
574071138	   0.0288		10		buy
55878265	   0.3		    10		sell
2022013720	   0		    10		buy
248081	       0.06		    10		sell
1055195011	   0.06		    10		sell
569612801	   0.06		    10		sell
1732254740	   0		    10		buy
1917074042	   0.0072	    10		buy
193476283	   3		    10		sell
282475249	   100		    10		sell
695744493	   3		    10		sell
742592951	   0.06		    10		sell
465646823	   3		    10		sell
1912264755	   5		    10		sell
1622650073	   100		    10		sell


limit_map: 
------------
price      volume       num_orders 	 order_ids
0.0288	    10		    1	    	 574071138 
0	        10		    2		     1732254740 2022013720 
3	        10		    3		    193476283 465646823 695744493 
0.0072	    10		    1		    1917074042 
0.3	        10		    1		    55878265 
0.06	    10		    4		 1055195011 742592951 569612801 248081 
5	        10		    1		 1912264755 
100	        10		    2		 282475249 1622650073 


Printing list NEW...
0:          1732254740/buy/10/0.000000 2022013720/buy/10/0.000000 
0.0072:     1917074042/buy/10/0.007200 
0.0288:     574071138/buy/10/0.028800 
0.06:       1055195011/sell/10/0.060000 742592951/sell/10/0.060000 569612801/sell/10/0.060000 248081/sell/10/0.060000 
0.3:        55878265/sell/10/0.300000 
3:          193476283/sell/10/3.000000 465646823/sell/10/3.000000 695744493/sell/10/3.000000 
5:          1912264755/sell/10/5.000000 
100:        282475249/sell/10/100.000000 1622650073/sell/10/100.000000 

----------------------------------------------------

----------------
order_id: 1566267953
order_type: sell
shares: 53
bid_price: 0.0108
curr_time: 1689130725676
----------------
_________incoming order________
| shares: 10
| order_type: sell
| limit: 0.0108
| order_id: 1566267953

new sell order has crossed the spread ...
need to find and return the most appropriate limit for the new order to match with. 
most appropriate limit will be: 0.0288
attempting to create a match...
perfect match between buyer (574071138) and seller (1566267953)
deleting old order...574071138
deleting incoming order... 1566267953

limit node is all out of orders! DELETING LIMIT NODE
[1]    76590 segmentation fault  ./socket



*/





// i think this should be taken care of now ....
/*

buy @ 0
buy @ 0.06
buy @ 6.00
buy @ 6.00
buy @ 8.00
buy @ 8.00

sell @ 100
sell @ 100

> sell @ 0.09




HIGHEST BUY OFFER: 8
LOWEST SELL OFFER: 100

order_map: 
------------
order_id	   limit        qantity		order_type
156288720	   0	         10	    	buy
1930150786	   0.06	         10	    	buy
1481299816	   6	         10	    	buy
1970383738	   8	         10	    	buy
2041647826	   6	         10	    	buy
1140563598	   8	         10	    	buy
1622650073	   100	         10	    	sell
282475249	   100	         10	    	sell


limit_map: 
------------
price      volume       num_orders 	 order_ids
0	        10		        1		 156288720 
0.06	    10		        1		 1930150786 
8	        10		        2		 1140563598 1970383738 
6	        10		        2		 2041647826 1481299816 
100	        10		        2		 282475249  1622650073 

Printing list NEW...
0:          156288720/buy/10/0.000000 
0.06:       1930150786/buy/10/0.060000 
6:          2041647826/buy/10/6.000000      1481299816/buy/10/6.000000 
8:          1140563598/buy/10/8.000000      1970383738/buy/10/8.000000 
100:        282475249/sell/10/100.0000      1622650073/sell/10/100.000000 

----------------------------------------------------

_________incoming order________
| shares: 10
| order_type: sell
| limit: 0.09
| order_id: 372016759


new sell order has crossed the spread ...
attempting to create a match...
incoming sell order crossed the spread, but doesnt have a limit to create matches with.
deleting head limit node @ price: 0.09
changing limit node and trying again...
attempting to create a match...
perfect match between buyer (2041647826) and seller (372016759)
deleting old order...2041647826
deleting incoming order... 372016759
[1]    65354 segmentation fault  ./socket


*/ 


// TODO
/*
test to prove that ...


int order_id_1 = this->create_order(book, "buy", 10, 13.00);
int order_id_2 = this->create_order(book, "buy", 10, 14.00);
int order_id_3 = this->create_order(book, "buy", 10, 15.00);
// SPREAD
int order_id_4 = this->create_order(book, "sell", 10, 17.00);
int order_id_5 = this->create_order(book, "sell", 10, 18.00);
int order_id_6 = this->create_order(book, "sell", 10, 19.00);
//TRIGGER
int order_id_7 = this->create_order(book, "buy", 10, 16.00);


can properly update 
*/