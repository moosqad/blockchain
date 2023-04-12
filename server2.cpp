
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

int main(int argc, char* argv[]) {
  // Set up the blockchain
  Blockchain blockchain(4);

  // Set up the server
  asio::io_context io_context;
  tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));
  int index = 1;

  // Serve requests
  while (true) {
    // Wait for a new connection
    tcp::socket socket(io_context);
    acceptor.accept(socket);
    std::cout << "Client connected from: " << socket.remote_endpoint()
              << std::endl;
    // Read the HTTP request
    beast::flat_buffer buffer;
    http::request<http::string_body> request;
    http::read(socket, buffer, request);

    // Handle the request
    http::response<http::string_body> response;
    try {
      if (request.method() == http::verb::post) {
        cout << "POST: ";
        // Parse the transaction data from the JSON request body
        nlohmann::json json_data = nlohmann::json::parse(request.body().data());
        std::string sender = json_data["sender"];
        std::string receiver = json_data["receiver"];
        float amount = json_data["amount"];
        std::string signature = json_data["signature"];

        // Add the transaction to the blockchain
        blockchain.addBlock(Block(
            index, {Transaction(sender, receiver, amount, signature)}, ""));
        index++;

        // Send a success response
        response.result(http::status::ok);
        response.set(http::field::content_type, "text/plain");
        response.body() = "Transaction added to the blockchain.";
        http::write(socket, response);  // move this line inside the try block
      } else if (request.method() == http::verb::get &&
                 request.target() == "/get_blockchain") {
        // Get the blockchain data
        std::string blockchain_data = blockchain.toString();

        // Send the blockchain data as a JSON response
        response.result(http::status::ok);
        response.set(http::field::content_type, "application/json");
        response.body() = blockchain_data;
        http::write(socket, response);  // move this line inside the try block
      } else if (request.method() == http::verb::get &&
                 request.target() == "/get_hello") {
        // handle get_hello request
        response.result(http::status::ok);
        response.set(http::field::content_type, "text/plain");
        response.body() = "Hello, world!";
      } else {
        cout << "Don't recongize request type" << endl;
        // Send a bad request response
        response.result(http::status::bad_request);
        response.set(http::field::content_type, "text/plain");
        response.body() = "Invalid request.";
        http::write(socket, response);  // move this line inside the try block
      }
    } catch (const std::exception& ex) {
      // Send an internal server error response
      response.result(http::status::internal_server_error);
      response.set(http::field::content_type, "text/plain");
      response.body() = "Internal Server Error: " + std::string(ex.what());
    } catch (const std::exception& ex) {
      // Send an internal server error response
      response.result(http::status::internal_server_error);
      response.set(http::field::content_type, "text/plain");
      response.body() = "Internal Server Error: " + std::string(ex.what());
    }

    // Respond to the client with the response message
    http::write(socket, response);

    // Shutdown the connection
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
