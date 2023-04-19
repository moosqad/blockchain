
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

#include "charity.cpp"

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
using tcp = asio::ip::tcp;
using namespace std;

int main() {
  sqlite3* db;
  if (sqlite3_open("charity.db", &db) != SQLITE_OK) {
    std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
    return 1;
  }
  Filial filial(db);
  Worker worker(db);
  Help help(db);
  help.createHelp("sdfs", "", "", "", 12, 1);
  Volunteer volunteer(db);
  Contractor contractor(db);
  Contract contract(db);

  // Set up the server
  asio::io_context io_context;
  tcp::acceptor acceptor(
      io_context, tcp::endpoint(asio::ip::make_address("0.0.0.0"), 8000));

  // int index = 1;

  // Ожидание соединения
  while (true) {
    // Ждем нового подключения
    tcp::socket socket(io_context);
    acceptor.accept(socket);
    std::cout << "Client connected from: " << socket.remote_endpoint()
              << std::endl;
    // Читаем HTTP запрос
    beast::flat_buffer buffer;
    http::request<http::string_body> request;
    http::read(socket, buffer, request);

    // Обработка HTTP запроса
    http::response<http::string_body> response;
    try {
      if (request.method() == http::verb::post &&
          request.target() == "/add_transaction") {
        cout << "POST: ";
        // Парсинг данных запроса в JSON формат

      } else if (request.method() == http::verb::get &&
                 request.target() == "/read") {
        string data = help.readHelp();

        response.result(http::status::ok);
        response.set(http::field::content_type, "application/json");
        response.body() = data;
        http::write(socket, response);
      } else {
        cout << "Don't recongize request type" << endl;
        // Отправляем ошибку
        response.result(http::status::bad_request);
        response.set(http::field::content_type, "text/plain");
        response.body() = "Invalid request.";
        http::write(socket, response);
      }
    } catch (const std::exception& ex) {
      // Отправляем внутреннюю ошибку сервера
      response.result(http::status::internal_server_error);
      response.set(http::field::content_type, "text/plain");
      response.body() = "Internal Server Error: " + std::string(ex.what());
    }

    // закрываем соединение
    beast::error_code ec;
    socket.shutdown(tcp::socket::shutdown_send, ec);
    if (ec == beast::errc::not_connected) {
      // Ignore the error if the socket was already closed
      ec = {};
    }
    if (ec) {
      throw beast::system_error{ec};
    }
  }

  return 0;
}
