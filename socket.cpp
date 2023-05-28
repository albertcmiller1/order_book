#include "socket.hpp"

std::unordered_map<std::string, char*> parse_args(int argc, char** argv){
    std::unordered_map<std::string, char*> arg_map = {};
    for (int i = 1; i < argc; ++i){
        if (!strcmp("--order_id", argv[i])){
            arg_map["order_id"] = argv[i+1];
        } else if (!strcmp("--limit", argv[i])){
            arg_map["limit"] = argv[i+1];
        } else if (!strcmp("--shares", argv[i])){
            arg_map["shares"] = argv[i+1];
        } else if (!strcmp("--buy_sell", argv[i])){
            // validate string == buy or sell 
            arg_map["buy_sell"] =  argv[i+1];
        } else if (!strcmp("--event_time", argv[i])){
            arg_map["event_time"] = argv[i+1];
        } else if (!strcmp("--entry_time", argv[i])){
            arg_map["entry_time"] = argv[i+1];
        }
    }

    /*
    if (arg_map.find("order_id") == arg_map.end()) {
        // this price is not in the limit map-- update limit_map. 
    } else {
        // this price is already in the limit_map. 
    }
    */

    return arg_map;
}

void start_socket_server(OrderBook book){
    crow::SimpleApp app;

    std::mutex mtx;
    std::unordered_set<crow::websocket::connection*> users;

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
          for (auto user : users)
              if (is_binary)
                  user->send_binary(data);
              else {
                  std::cout << "data: " << data << std::endl;
                  user->send_text(data);
              }
      });

    CROW_ROUTE(app, "/")([](){
        return "Hello world\n";
    });

    app.port(5001)
      .multithreaded()
      .run();
}

void trading_bot(OrderBook book){
    while (true){
        sleep(5);  
        std::cout << "THREAD still working" << std::endl;
       
        // book.add_order(
        //     000003,         // order_id
        //     true,           // buy_sell
        //     1,              // shares
        //     23.44,          // limit
        //     983485,         // entry_time
        //     983485          // event_time
        // );
    }
}

int main(){
    std::cout << "Hello order boook\n";
    OrderBook book;

    std::cout << "starting trading bot threads 1\n";
    std::thread th1(trading_bot, book);

    std::cout << "all bots up. starting webserver\n";
    start_socket_server(book);
}
