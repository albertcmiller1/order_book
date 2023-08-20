#include "socket.hpp"
#define CROW_JSON_USE_MAP
using namespace std;
std::unordered_set<crow::websocket::connection*> users;
std::mutex mtx;


static std::random_device              rd;
static std::mt19937                    gen(rd());
static std::uniform_int_distribution<> dis(0, 15);
std::string generate_order_id(OrderBook *book) {
    std::stringstream ss;
    int i;
    ss << std::hex;
    for (i = 0; i < 10; i++) { ss << dis(gen); }
    std::string order_id = ss.str();
    // i think this can just be an if statement? 
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
            thread_id,  
            shares,            
            offer,             
            curr_time
        );
        // std::cout << *book << std::endl;
        // std::cout << "<--------------------------------------------------------------------------------------------------------------------------------------------------------------------------->\n\n\n";
        cnt++;
        m.unlock();
    }
}

void send_spread_to_users(OrderBook *book, int sleep_time){
    while (true){
        sleep(sleep_time);
        m.lock();
        std::string spread_data = book->get_spread_data();
        for (auto user : users) { user->send_text(spread_data); }
        m.unlock();
    }
}

void send_curr_price_to_users(OrderBook *book, int sleep_time){
    while (true){
        sleep(sleep_time);
        std::string curr_price = to_string(book->most_recent_trade_price);
        for (auto user : users) {
            user->send_text(curr_price);
        }
    }
}

void create_socket(OrderBook *book, crow::SimpleApp &app){
    // might want multiple wesbocket routs? 
    // each book should have a unique websocket rout -> might need to move this over to the book itself. 
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
        // no reason for users to send messages         
    });

    CROW_ROUTE(app, "/ipo")([](){
        // create a book, add to set, start trading bot threads @ ipo price
        std::string ticker;
        std::unordered_map<std::string, OrderBook*> books;  // ticker, associated book
        return "Hello world\n";
    });
}

void create_cancel_order_route(OrderBook *book, crow::SimpleApp &app){
    CROW_ROUTE(app, "/cancel_order")
    .methods("POST"_method)
    ([book](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400);
        string      order_id    = x["order_id"].s();
        std::string status = "";
        m.lock();
        if (book->order_map.count(order_id)){
            std::cout << "canceling order...\n";
            // book->cancel_order()
            status = "canceled";
        } else {
            status = "filled";
        }
        m.unlock();

        crow::json::wvalue z({
            {"order_id", order_id},
            {"status", status}
        });
        return crow::response(z);
    });
}

void create_order_status_route(OrderBook *book, crow::SimpleApp &app){
    CROW_ROUTE(app, "/order_status")
    .methods("POST"_method)
    ([book](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400);

        string      order_id    = x["order_id"].s();
        std::cout << "checking on status of order...\n";
        std::string status = "";
        m.lock();
        if (book->order_map.count(order_id)){
            status = "pending";
        } else {
            status = "filled";
        }
        m.unlock();

        crow::json::wvalue res({
            {"order_id", order_id},
            {"status", status}
        });
        return crow::response(res);
    });
}

void create_get_spread_route(OrderBook *book, crow::SimpleApp &app){
    CROW_ROUTE(app, "/spread")
    ([]{
        crow::json::wvalue x({
            {"buy-4", "hi"},
            {"buy-5", "hi"},
            {"sell-1", "hi"}
        });
        return x;
    });
}

void create_get_curr_price_route(OrderBook *book, crow::SimpleApp &app){
    CROW_ROUTE(app, "/curr_price")
    ([book] {
        return to_string(book->most_recent_trade_price);
    });
}

void create_place_order_route(OrderBook *book, crow::SimpleApp &app){
    CROW_ROUTE(app, "/place_order")
    .methods("POST"_method)
    ([book](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400);

        string      order_type  = x["order_type"].s();
        string      user_id     = x["user_id"].s();
        double      price       = x["price"].d();
        int         shares      = x["shares"].i();
        string      order_id    = generate_order_id(book);
        uint64_t    curr_time   = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        std::vector<std::string> v = {"buy", "sell"};
        if (!std::count(v.begin(), v.end(), order_type)) { return crow::response(400); }

        if (false){
            std::cout << "placing order...\n";
            std::cout << "  >> user_id: "       << user_id << std::endl;
            std::cout << "  >> price: "         << price << std::endl;
            std::cout << "  >> order_type: "    << order_type << std::endl;
            std::cout << "  >> shares: "        << shares << std::endl;
            std::cout << "  >> curr_time: "     << curr_time << std::endl;
        }

        m.lock();
        book->add_order(
            order_id,            
            order_type,          
            user_id,             
            shares,              
            price,               
            curr_time                          
        );
        // maybe book->add_order returns an array of matches that were created. 
        // post in dynmao
        // send out via socket 

        // cout << "===========================================\n";
        std::cout << *book << std::endl;
        // cout << "===========================================\n";

        m.unlock();

        crow::json::wvalue z({
            {"order_placed", "true"},
            {"trade_price", price},
            {"shares", shares},
            {"user_id", user_id},
            {"user_id", user_id}
        });
        return crow::response(z);
    });
}

void start_crow_app(OrderBook *book, int crow_port){
    crow::SimpleApp app;

    create_socket(book, app);
    create_cancel_order_route(book, app);
    create_order_status_route(book, app);
    create_get_spread_route(book, app);
    create_get_curr_price_route(book, app);
    create_place_order_route(book, app);

    app.port(crow_port).multithreaded().run();
}

int main(){
    OrderBook *book = new OrderBook;
    std::vector<std::thread> threads;

    threads.push_back(std::thread(trading_bot, book, "th1"));
    threads.push_back(std::thread(start_crow_app, book, 5001));
    threads.push_back(std::thread(send_spread_to_users, book, 3));

    for(auto &thread : threads){ thread.join(); }

    // wscat -c ws://0.0.0.0:5001/ws
    // curl http://0.0.0.0:5001/curr_price
}
