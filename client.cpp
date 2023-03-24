#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>

using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {
  try {
    // Get the transaction details from command line arguments
    if (argc != 4) {
      std::cerr << "Usage: client <sender> <receiver> <amount>" << std::endl;
      return 1;
    }

    std::string sender = argv[1];
    std::string receiver = argv[2];
    float amount = std::stof(argv[3]);

    // Create an io_context object
    boost::asio::io_context io_context;

    // Create a tcp::resolver object to resolve the server address
    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints =
        resolver.resolve("127.0.0.1", "8080");

    // Create a tcp::socket object and connect to the server
    tcp::socket socket(io_context);
    boost::asio::connect(socket, endpoints);

    // Set the SO_REUSEADDR option on the socket
    int yes = 1;
    if (setsockopt(socket.native_handle(), SOL_SOCKET, SO_REUSEADDR, &yes,
                   sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }

    // Form the transaction message and send it to the server
    std::string message =
        sender + " sent " + std::to_string(amount) + " coins to " + receiver;
    boost::asio::write(socket, boost::asio::buffer(message));

    // Read the response from the server
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, '\n');

    // Print the response
    std::string response_data =
        boost::asio::buffer_cast<const char*>(response.data());
    std::cout << response_data << std::endl;

    return 0;
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
}
