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

    cout << arg_map["limit"] << endl;
    cout << arg_map["user_id"] << endl;
    cout << arg_map["shares"] << endl;
    cout << arg_map["order_type"] << endl;

    return arg_map;
}

std::unordered_set<crow::websocket::connection*> users;

void trading_bot(OrderBook &book){
    while (true){
        sleep(5);  
        std::cout << "THREAD still working" << std::endl;

        for (auto user : users)
            // std::cout << "data: " << data << std::endl;
            user->send_text("THREAD still working");
       
        srand((unsigned) time(NULL));
        int order_id = rand();
        unsigned int microseconds {10000};

        uint64_t curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        order_id = rand();
        usleep(microseconds);
        curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        book.add_order(
            order_id,           // order_id
            "sell",             // order_type
            3,                  // shares
            23.47,              // limit
            curr_time,          // entry_time
            curr_time           // event_time
        );
    }
}

void start_socket_server(OrderBook &book){

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
            std::unordered_map<std::string, std::string> arg_map = parse_args(V);

            book.add_order(
                11111111,                                   // int order_id
                arg_map["order_type"],                      // bool order_type
                stoi(arg_map["shares"]),                    // int shares
                stof(arg_map["limit"]),                     // float limit
                66666666,                                   // int entry_time
                99999999                                    // int event_time
            );

            std::cout << book << std::endl;
            std::cout << "----------------------------------------------------\n\n";

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

int main(){
    OrderBook book;

    // std::cout << "starting trading bot threads...\n";
    // std::thread th1(trading_bot, book);
    // std::cout << "all threads up and running.\n";

    // wscat -c ws://0.0.0.0:5001/ws
    // --limit 23.42 --shares 3 --order_type buy --user_id albert
    std::cout << "starting webserver...\n";
    start_socket_server(book);

    return 0;
}
