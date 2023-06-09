#include "socket.hpp"
using namespace std;

std::unordered_map<std::string, std::string> parse_args(vector <std::string> v){
    std::unordered_map<std::string, std::string> arg_map = {};

    for (int i = 0; i < v.size(); i++){
        if (v[i] == "--limit"){
            arg_map["limit"] = v[i+1];
        } else if (v[i] == "--user_id") {
            arg_map["user_id"] = v[i+1];
        } else if (v[i] == "--shares") {
            arg_map["shares"] = v[i+1];
        } else if (v[i] == "--order_type") {
            arg_map["order_type"] = v[i+1];
        }
    }

    srand((unsigned) time(NULL));
    int order_id = rand();
    uint64_t curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    arg_map["order_id"] = to_string(order_id);
    arg_map["entry_time"] = curr_time;

    cout << arg_map["limit"] << endl;
    cout << arg_map["user_id"] << endl;
    cout << arg_map["shares"] << endl;
    cout << arg_map["order_type"] << endl;

    return arg_map;
}

int count_digit(int number) {
    int count = 0;
    while(number != 0) {
        number = number / 10;
        count++;
    }
    return count;
}

bool validate_args(std::unordered_map<std::string, std::string> arg_map){

    // std::string s = arg_map["limit"];
    // for( int i = 0; i < s.length(); i++ ) {
    //     if( !isdigit( s[i] ) !=) {
    //         return false;
    //     }
    // }

    return true;
}

std::unordered_set<crow::websocket::connection*> users;

void trading_bot(OrderBook *book){
    while (true){
        sleep(1);  

        // std::cout << "THREAD still working" << std::endl;
        // for (auto user : users){
        //     // std::cout << "data: " << data << std::endl;
        //     user->send_text("THREAD still working");
        // }
       
        // srand((unsigned) time(NULL));
        uint64_t curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        int order_id = rand();
        
        while (count_digit(order_id) <= 9){ 
            order_id *= 10;
        }
        
        std::string order_type;
        if (order_id %2){
            order_type = "buy";
        } else {
            order_type = "sell";
        }

        int opt = order_id % 4; 
        double bid_price;
        double factor = (order_id % 100) * 0.01;
        if (book->most_recent_trade_price){
            if (opt == 0){
                bid_price = book->most_recent_trade_price + factor;
            } else if (opt == 1){
                bid_price = book->most_recent_trade_price - factor;
            } else if (opt == 2){
                bid_price = book->most_recent_trade_price + factor;
            } else if (opt == 3){
                bid_price = book->most_recent_trade_price - factor;
            } else {
                bid_price = book->most_recent_trade_price + factor;
            }
        } else {
            bid_price = 100;
        }

        int shares = order_id%100;

        curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        // cout << "----------------\n";
        // cout << "order_id: "    << order_id << endl;
        // cout << "order_type: "  << order_type << endl;
        // cout << "shares: "      << shares << endl;
        // cout << "bid_price: "   << bid_price << endl;
        // cout << "curr_time: "   << curr_time << endl;
        // cout << "----------------\n";

        book->add_order(
            order_id,           // order_id
            order_type,         // order_type
            10,                 // shares
            bid_price,                // limit
            curr_time,          // entry_time
            curr_time           // event_time
        );

        std::cout << *book << std::endl;
        std::cout << "----------------------------------------------------\n\n";
    }
}

void start_socket_server(OrderBook *book){
    crow::SimpleApp app;
    std::mutex mtx;

    CROW_WEBSOCKET_ROUTE(app, "/ws")
      .onopen([&](crow::websocket::connection& conn) {
          CROW_LOG_INFO << "new websocket connection from " << conn.get_remote_ip();
          std::lock_guard<std::mutex> _(mtx);
          users.insert(&conn);
      })
      .onclose([&](crow::websocket::connection& conn, const std::string& reason) {
          CROW_LOG_INFO << "websocket connection closed: " << reason;
          std::lock_guard<std::mutex> _(mtx);
          users.erase(&conn);
      })
      .onmessage([&](crow::websocket::connection& /*conn*/, const std::string& data, bool is_binary) {
          std::lock_guard<std::mutex> _(mtx);
        
            // parse message and create order  
            vector<std::string> V; 
            istringstream iss(data);   
            string word;
            while(iss>>word){V.push_back(word);}

            for (auto i : V){
                std::cout << i << endl;
            }

            std::unordered_map<std::string, std::string> arg_map = parse_args(V);

            if (validate_args(arg_map)){
                book->add_order(
                    11111111,                                   // int order_id
                    arg_map["order_type"],                      // bool order_type
                    stoi(arg_map["shares"]),                    // int shares
                    stof(arg_map["limit"]),                     // double limit
                    66666666,                                   // int entry_time
                    99999999                                    // int event_time
                );
            } else {
                std::cout << "invalid input" << endl;
            }

            // std::cout << 11111111               << endl;                   
            // std::cout << arg_map["order_type"]  << endl;                   
            // std::cout << stoi(arg_map["shares"]) << endl;                   
            // std::cout << stof(arg_map["limit"]) << endl;                   
            // std::cout << 66666666               << endl;                   
            // std::cout << 99999999                << endl;                   


            // std::cout << *book << std::endl;
            // std::cout << "BOOOG----------------------------------------------------GIIIIE\n\n";

            // broadcast message to all connectued users 
            // for (auto user : users)
            //     if (is_binary)
            //         user->send_binary(data);
            //     else {
            //         std::cout << "data: " << data << std::endl;
            //         user->send_text(data);
            //     }
      });

    CROW_ROUTE(app, "/")([](){
        return "Hello world\n";
    });

    std::cout << "starting api + websocket...\n";
    app.port(5001)
      .multithreaded()
      .run();
}

void test(){
    while (true){
        sleep(4);  

        // srand((unsigned) time(NULL));
        uint64_t curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        int order_id = rand();
        
        std::string order_type;
        if (order_id %2){
            order_type = "sell";
        } else {
            order_type = "buy";
        }

        double bid_price;
        double most_recent_trade_price {100};

        int opt;
        opt = order_id % 4; 

        double factor;
        factor = (order_id % 100) * 0.01;
        std::cout << "mulip factor: " << factor << std::endl;
        std::cout << "opt: " << opt << std::endl;



        if (most_recent_trade_price){
            if (opt == 0){
                bid_price = most_recent_trade_price + factor;
            } else if (opt == 1){
                bid_price = most_recent_trade_price - factor;
            } else if (opt == 2){
                bid_price = most_recent_trade_price + factor;
            } else if (opt == 3){
                bid_price = most_recent_trade_price - factor;
            } else {
                bid_price = most_recent_trade_price + factor;
            }
        } else {
            bid_price = 100;
        }


        std::cout << "bid_price: " << bid_price << std::endl;

        int shares = order_id%100;

        curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        cout << "----------------\n";
        cout << "order_id: "    << order_id << endl;
        cout << "order_type: "  << order_type << endl;
        cout << "shares: "      << shares << endl;
        cout << "bid_price: "   << bid_price << endl;
        cout << "curr_time: "   << curr_time << endl;
        cout << "----------------\n";

    }

}

int main(){
    OrderBook *book = new OrderBook;

    std::cout << "starting trading bot threads...\n";
    std::thread th1(trading_bot, book);
    std::cout << "all threads up and running.\n";

    // wscat -c ws://0.0.0.0:5001/ws
    // --limit 23.42 --shares 3 --order_type buy --user_id albert
    std::cout << "starting webserver...\n";
    start_socket_server(book);

    return 0; 
}
