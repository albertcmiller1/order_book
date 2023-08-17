#include "socket.hpp"
#define CROW_JSON_USE_MAP
using namespace std;

static std::random_device              rd;
static std::mt19937                    gen(rd());
static std::uniform_int_distribution<> dis(0, 15);
std::string generate_order_id(OrderBook *book) {
    std::stringstream ss;
    int i;
    ss << std::hex;
    for (i = 0; i < 10; i++) { ss << dis(gen); }
    std::string order_id = ss.str();
    while (book->order_map.find(order_id) != book->order_map.end()){
        // order id already exists. try a new one.
        order_id = generate_order_id(book);
    }
    return order_id;
}

std::mutex m;
std::atomic<int> cnt = 1;
void trading_bot(OrderBook *book, std::string thread_id){
    /*
        TODO:
        > bot should realize how many orders/limits there are. 
        > keep set limits on most_recent_trade price so stock price doesnt go negative or blow up
        >> if the number of orders/limits is getting too big, slightly influence the book to create matches so we dont run out of memory 
    */

    double ipo = 100.00;

    while (true){
        sleep(0);  

        uint64_t curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        std::string order_id = generate_order_id(book);
        if (book->order_map.find(order_id) != book->order_map.end()){std::cout << "shit\n"; throw;}

        double offer;
        int rand_num = rand();
        std::string order_type;
        if (rand_num % 2){ order_type = "buy";} else { order_type = "sell"; }

        if (book->most_recent_trade_price){
            int opt = rand_num % 10; 
            double factor = 0.01;
            offer = ipo * factor;

            if      (opt == 0)  {offer = book->most_recent_trade_price + factor;} 
            else if (opt == 1)  {offer = book->most_recent_trade_price - factor;} 
            else if (opt == 2)  {offer = book->most_recent_trade_price + factor;} 
            else if (opt == 3)  {offer = book->most_recent_trade_price - factor;} 
            else if (opt == 4)  {offer = book->most_recent_trade_price + factor;} 
            else if (opt == 5)  {offer = book->most_recent_trade_price + factor + 0.01;} 
            else if (opt == 6)  {offer = book->most_recent_trade_price + factor + 0.01;} 
            else                {offer = book->most_recent_trade_price;}
        } else {
            offer = ipo;
        }

        int shares = rand_num%100;
        curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        m.lock();
        // std::cout << "<--------------------------------------------------------------------------" << thread_id << "-" << cnt << "--------------------------------------------------------------------------------------->\n";
        book->add_order(
            order_id,           
            order_type,        
            shares,            
            offer,             
            curr_time,         
            curr_time          
        );
        // std::cout << *book << std::endl;
        // std::cout << "<--------------------------------------------------------------------------------------------------------------------------------------------------------------------------->\n\n\n";
        cnt++;
        m.unlock();
    }
}

void send_spread_to_users(OrderBook *book){
    while (true){
        sleep(5);
        std::string spread_data = "spread info";
        for (auto user : book->users) {
            user->send_text(spread_data);
        }
    }
}

void send_curr_price_to_users(OrderBook *book){
    while (true){
        sleep(5);
        std::string curr_price = to_string(book->most_recent_trade_price);
        for (auto user : book->users) {
            user->send_text(curr_price);
        }
    }
}

void start_socket_server(OrderBook *book, int crow_port){
    crow::SimpleApp app;
    std::mutex mtx;

    // might want multiple wesbocket routs? 
    // each book should have a unique websocket rout -> might need to move this over to the book itself. 
    CROW_WEBSOCKET_ROUTE(app, "/ws")
      .onopen([&](crow::websocket::connection& conn) {
          CROW_LOG_INFO << "new websocket connection from " << conn.get_remote_ip();
          std::lock_guard<std::mutex> _(mtx);
          book->users.insert(&conn);
      })
      .onclose([&](crow::websocket::connection& conn, const std::string& reason) {
          CROW_LOG_INFO << "websocket connection closed: " << reason;
          std::lock_guard<std::mutex> _(mtx);
          book->users.erase(&conn);
      })
      .onmessage([&](crow::websocket::connection& /*conn*/, const std::string& data, bool is_binary) {
          std::lock_guard<std::mutex> _(mtx);
            // no reason for users to send messages         
      });

    CROW_ROUTE(app, "/ipo")([](){
        // create a book, add to set, start trading bot threads @ ipo price
        std::string ticker;
        std::unordered_map<std::string, OrderBook*> books;  // ticker, associated book
        return "Hello world\n";
    });

    CROW_ROUTE(app, "/cancel_order")([](){
        // build function to remove order from limit's dll and order_map
        return "Hello world\n";
    });
    
    CROW_ROUTE(app, "/order_status")([](){
        // show order type, current trading price
        return "Hello world\n";
    });

    CROW_ROUTE(app, "/spread")
    ([]{
        crow::json::wvalue x({
            {"spread-1", "hi"},
            {"spread-2", "Hello, World2!"}
        });
        return x;
    });

    CROW_ROUTE(app, "/curr_price")
    ([book] {
        return to_string(book->most_recent_trade_price);
    });

    CROW_ROUTE(app, "/place_order")
    .methods("POST"_method)
    ([book](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400);

        std::string order_type = x["order_type"].s();
        double price = x["price"].d();
        int shares = x["shares"].i();

        std::cout << "price: " << price << std::endl;
        std::cout << "order_type: " << order_type << std::endl;
        std::cout << "shares: " << shares << std::endl;

        std::string order_id = generate_order_id(book);
        if (book->order_map.find(order_id) != book->order_map.end()){ std::cout << "shit\n"; throw; }
        m.lock();
        book->add_order(
            order_id,                                   // string order_id
            order_type,                                 // string order_type
            shares,                                     // int shares
            price,                                      // double limit price 
            66666666,                                   // int entry_time
            99999999                                    // int event_time
        );
        m.unlock();

        crow::json::wvalue z({
            {"order_placed", "true"},
            {"trade_price", "103.22"}
        });
        return crow::response(z);
    });

    app.port(crow_port).multithreaded().run();
}

int main(){
    OrderBook *book = new OrderBook;
    std::vector<std::thread> threads;
    int crow_port = 5001;

    threads.push_back(std::thread(trading_bot, book, "th1"));
    threads.push_back(std::thread(start_socket_server, book, crow_port));

    for(auto &thread : threads){ thread.join(); }

    // wscat -c ws://0.0.0.0:5001/ws
    // curl http://0.0.0.0:5001 
    // nohup 
}
