
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

#include "blockchain.cpp"

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
using tcp = asio::ip::tcp;
using namespace std;

int main() {
  Blockchain blockchain(4);

  // Set up the server
  asio::io_context io_context;
  tcp::acceptor acceptor(
      io_context, tcp::endpoint(asio::ip::make_address("0.0.0.0"), 8080));

  int index = 1;

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
        nlohmann::json json_data = nlohmann::json::parse(request.body().data());
        std::string sender = json_data["sender"];
        std::string receiver = json_data["receiver"];
        float amount = json_data["amount"];

        // Добавляем транзакцию в блокчейн
        blockchain.addBlock(
            Block(index, {Transaction(sender, receiver, amount)}, ""));
        index++;

        // Отправляем ответ
        response.result(http::status::ok);
        response.set(http::field::content_type, "text/plain");
        response.body() = "Transaction added to the blockchain.";
        http::write(socket, response);
      } else if (request.method() == http::verb::post &&
                 request.target() == "/get_sum") {
        cout << "POST: ";
        // Парсинг данных запроса в JSON формат
        nlohmann::json json_data = nlohmann::json::parse(request.body().data());
        std::string receiver = json_data["receiver"];
        double ans = blockchain.getReceiverTotal(receiver);
        // Добавляем транзакцию в блокчейн
        nlohmann::json json_result = {{"response", ans}};

        response.result(http::status::ok);
        response.set(http::field::content_type, "application/json");
        response.body() = json_result.dump();
        http::write(socket, response);

      } else if (request.method() == http::verb::get &&
                 request.target() == "/is_valid") {
        bool is_valid = blockchain.isValid(blockchain.db);
        nlohmann::json json_result = {
            {"is_valid", is_valid},
            {"message",
             is_valid
                 ? "Цепочка блокчейна не подвергалась "
                   "изменениям извне. Блокчейну можно доверять"
                 : "ВНИМАНИЕ! Цепь блокчейна подверглась изменениям со стороны "
                   "администраторов фонда. Не совершайте транзакции!"}};

        response.result(http::status::ok);
        response.set(http::field::content_type, "application/json");
        response.body() = json_result.dump();
        http::write(socket, response);
      } else if (request.method() == http::verb::get &&
                 request.target() == "/get_blockchain") {
        json blockchain_data = blockchain.toJSON();

        response.result(http::status::ok);
        response.set(http::field::content_type, "application/json");
        response.body() = blockchain_data.dump(2);
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
