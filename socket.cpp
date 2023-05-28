#include "crow.h"
#include <unordered_set>
#include <mutex>


int main()
{
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
          for (auto u : users)
              if (is_binary)
                  u->send_binary(data);
              else {
                  std::cout << "data: " << data << std::endl;
                  u->send_text(data);
              }
      });

    CROW_ROUTE(app, "/")([](){
        return "Hello world\n";
    });

    app.port(5001)
      .multithreaded()
      .run();
}
