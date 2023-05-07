
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

#include "charity.cpp"
#include "registration.cpp"

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
using tcp = asio::ip::tcp;
using namespace std;

int main() {
  sqlite3* db;
  Users adm("", "", "", "", "", "admin", "admin");
  adm.add_user();
  if (sqlite3_open("charity.db", &db) != SQLITE_OK) {
    std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
    return 1;
  }
  Filial filial(db);
  Worker worker(db);
  Help help(db);
  help.createHelp("Иванов", "Иван", "Иванович", "Пропал", 12000, 1);
  help.createHelp("Петров", "Петр", "Петрович", "Сирота", 10000, 2);
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
      if (request.method() == http::verb::get && request.target() == "/read") {
        string data = help.readHelp();

        response.result(http::status::ok);
        response.set(http::field::content_type, "application/json");
        response.body() = data;
        http::write(socket, response);
      } else if (request.method() == http::verb::post &&
                 request.target() == "/add_user") {
        cout << "POST: ";

        nlohmann::json json_data = nlohmann::json::parse(request.body().data());
        std::string username = json_data["username"];
        std::string password = json_data["password"];
        std::string first_name = json_data["first_name"];
        std::string second_name = json_data["second_name"];
        std::string third_name = json_data["third_name"];
        std::string phone = json_data["phone"];
        std::string email = json_data["email"];

        Users new_user(first_name, second_name, third_name, phone, email,
                       username, password);

        bool is_exist = !new_user.user_exists();

        nlohmann::json json_result = {
            {"response", is_exist},
            {"message", is_exist ? "Вы успешно зарегестрировались"
                                 : "Пользователь с таким никнеймом уже "
                                   "существует. Попробуйте другой"}};

        if (is_exist) {
          new_user.add_user();
        }

        response.result(http::status::ok);
        response.set(http::field::content_type, "application/json");
        response.body() = json_result.dump();
        http::write(socket, response);
      } else if (request.method() == http::verb::post &&
                 request.target() == "/sign_in") {
        cout << "POST: ";

        nlohmann::json json_data = nlohmann::json::parse(request.body().data());
        std::string username = json_data["username"];
        std::string password = json_data["password"];
        Users new_user("", "", "", "", "", username, password);

        bool is_exist = new_user.is_valid_username_password();

        nlohmann::json json_result = {
            {"response", is_exist},
        };

        response.result(http::status::ok);
        response.set(http::field::content_type, "application/json");
        response.body() = json_result.dump();
        http::write(socket, response);
      } else if (request.method() == http::verb::post &&
                 request.target() == "/update") {
        cout << "POST: UPDATE:";
        // Парсинг данных запроса в JSON формат
        nlohmann::json req_body = nlohmann::json::parse(request.body());

        // Получаем параметры для обновления помощи
        int id = req_body["id"];
        std::string first_name = req_body["first_name"];
        std::string second_name = req_body["second_name"];
        std::string third_name = req_body["third_name"];
        std::string problem = req_body["problem"];
        int money_goal = req_body["money_goal"];
        int filial_id = req_body["filial_id"];

        // Обновляем информацию о помощи в базе данных
        help.updateById(id, first_name, second_name, third_name, problem,
                        money_goal, filial_id);

        // Возвращаем ответ клиенту
        response.result(http::status::ok);
        response.set(http::field::content_type, "text/json");
        response.body() = "Help information updated successfully.";
        http::write(socket, response);
      } else if (request.method() == http::verb::post &&
                 request.target() == "/get_balance") {
        cout << "Get: GET BALANCE:";
        // Парсинг данных запроса в JSON формат
        nlohmann::json req_body = nlohmann::json::parse(request.body());
        // Получаем параметры для обновления помощи

        std::string username = req_body["username"];
        // Users new_user(username, "", "", "", "", "", "");
        // Обновляем информацию о помощи в базе данных
        nlohmann::json json_result = {
            {"balance", adm.get_user_cash_amount(username)}};

        response.result(http::status::ok);
        response.set(http::field::content_type, "application/json");
        response.body() = json_result.dump();
        http::write(socket, response);
      } else if (request.method() == http::verb::post &&
                 request.target() == "/update_balance") {
        cout << "POST: GET BALANCE:";
        // Парсинг данных запроса в JSON формат
        nlohmann::json req_body = nlohmann::json::parse(request.body());
        // Получаем параметры для обновления помощи
        int amount = req_body["amount"];
        std::string username = req_body["username"];
        Users new_user("", "", "", "", "", "", "");
        // Обновляем информацию о помощи в базе данных
        new_user.update_cash_amount(amount, username);

        // Возвращаем ответ клиенту
        response.result(http::status::ok);
        response.set(http::field::content_type, "text/json");
        response.body() = "Cash balance updated successfully.";
        http::write(socket, response);

      } else if (request.method() == http::verb::post &&
                 request.target() == "/get_user_info") {
        cout << "POST: GET USER INFO:";
        // Парсинг данных запроса в JSON формат
        nlohmann::json req_body = nlohmann::json::parse(request.body());
        // Получаем параметры для обновления помощи
        std::string username = req_body["username"];
        Users new_user("", "", "", "", "", "", "");
        // Обновляем информацию о помощи в базе данных
        string user_info = new_user.get_user_info(username);

        // Возвращаем ответ клиенту
        response.result(http::status::ok);
        response.set(http::field::content_type, "text/json");
        response.body() = user_info;
        http::write(socket, response);
      } else if (request.method() == http::verb::post &&
                 request.target() == "/create") {
        cout << "POST: CREATE:";
        // Парсинг данных запроса в JSON формат
        nlohmann::json req_body = nlohmann::json::parse(request.body());

        // Получаем параметры для обновления помощи
        std::string first_name = req_body["first_name"];
        std::string second_name = req_body["second_name"];
        std::string third_name = req_body["third_name"];
        std::string problem = req_body["problem"];
        int money_goal = req_body["money_goal"];
        int filial_id = req_body["filial_id"];

        // Обновляем информацию о помощи в базе данных
        help.createHelp(first_name, second_name, third_name, problem,
                        money_goal, filial_id);

        // Возвращаем ответ клиенту
        response.result(http::status::ok);
        response.set(http::field::content_type, "text/json");
        response.body() = "Help information created successfully.";
        http::write(socket, response);
      } else if (request.method() == http::verb::post &&
                 request.target() == "/delete") {
        cout << "POST: DELETE:";
        // Парсинг данных запроса в JSON формат
        nlohmann::json req_body = nlohmann::json::parse(request.body());

        // Получаем параметры для обновления помощи
        int id = req_body["id"];

        // Обновляем информацию о помощи в базе данных
        help.deleteHelp(id);

        // Возвращаем ответ клиенту
        response.result(http::status::ok);
        response.set(http::field::content_type, "text/json");
        response.body() = "Help information deleted successfully.";
        http::write(socket, response);
      }

      else {
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
