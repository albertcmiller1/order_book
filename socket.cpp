#include "socket.hpp"
#define CROW_JSON_USE_MAP
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
        std::cout << "<--------------------------------------------------------------------------" << thread_id << "-" << cnt << "--------------------------------------------------------------------------------------->\n";
        book->add_order(
            order_id,           
            order_type,        
            shares,            
            offer,             
            curr_time,         
            curr_time          
        );
        std::cout << *book << std::endl;
        std::cout << "<------------------------------------------------------------------------------------------------------------------------------------------------------------------>\n\n\n";
        cnt++;
        m.unlock();
    }
}

void start_socket_server(OrderBook *book, int crow_port){
    crow::SimpleApp app;
    std::mutex mtx;

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
                std::string order_id = generate_order_id(book);
                if (book->order_map.find(order_id) != book->order_map.end()){ std::cout << "shit\n"; throw; }
                m.lock();
                book->add_order(
                    order_id,                                   // string order_id
                    arg_map["order_type"],                      // bool order_type
                    stoi(arg_map["shares"]),                    // int shares
                    stof(arg_map["limit"]),                     // double limit
                    66666666,                                   // int entry_time
                    99999999                                    // int event_time
                );
                m.unlock();
            } else {
                std::cout << "invalid input" << endl;
            }

            // std::cout << 11111111                    << endl;                   
            // std::cout << arg_map["order_type"]       << endl;                   
            // std::cout << stoi(arg_map["shares"])     << endl;                   
            // std::cout << stof(arg_map["limit"])      << endl;                   
            // std::cout << 66666666                    << endl;                   
            // std::cout << 99999999                    << endl;                   


            std::cout << *book << std::endl;

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
    
    CROW_ROUTE(app, "/json")
    ([]{
        crow::json::wvalue x({{"message", "Hello, World!"}});
        x["message2"] = "Hello, World.. Again!";
        return x;
    });

    CROW_ROUTE(app, "/json_2")
    ([] {
        crow::json::wvalue x({{"zmessage", "Hello, World!"},
                              {"amessage", "Hello, World2!"}});
        return x;
    });


    CROW_ROUTE(app, "/add_json")
    .methods("POST"_method)
    ([](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(crow::status::BAD_REQUEST); // same as crow::response(400)

        std::cout << x << std::endl;
        std::cout << typeid(x).name() << std::endl;
        std::cout << x["price"].d() << std::endl;
        std::cout << x["order_type"].s() << std::endl;

        double price = x["price"].d();
        std::string order_type = x["order_type"].s();
        std::cout << "price: " << price << std::endl;
        std::cout << "order_type: " << order_type << std::endl;


        crow::json::wvalue z({{"Order_placed", "true"},
                              {"trade_price", "103.22"}});
        return crow::response(z);
    });


    std::cout << "starting api + websocket on port: " << crow_port << std::endl;
    app.port(crow_port)
      .multithreaded()
      .run();
}

int main(){
    OrderBook *book = new OrderBook;

    // std::cout << "starting trading bot threads...\n";
    // std::thread th1(trading_bot, book, "th1");
    // std::thread th2(trading_bot, book, "th2");
    // std::thread th3(trading_bot, book, "th3");
    // std::cout << "all threads up and running.\n";

    // wscat -c ws://0.0.0.0:5001/ws
    // curl http://0.0.0.0:5001 
    // --limit 23.42 --shares 3 --order_type buy --user_id albert

    std::cout << "starting webserver...\n";
    start_socket_server(book, 5001);

    return 0; 
}
