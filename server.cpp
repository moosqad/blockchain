#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>

#include "blockchain.cpp"

using boost::asio::ip::tcp;
// Define a vector to hold all connected clients
std::vector<tcp::socket> connectedClients;
// Define a function to handle incoming client connections
void handleConnection(tcp::socket socket, Blockchain& blockchain) {
  try {
    std::cout << "New client connected." << std::endl;

    // Define a buffer to hold incoming data
    char data[1024];

    // Read the incoming data from the socket
    size_t len = socket.read_some(boost::asio::buffer(data, 1024));

    // Parse the incoming data to get the transaction details
    std::string txString(data, len);
    std::istringstream iss(txString);
    std::string sender, receiver;
    float amount;
    iss >> sender >> receiver >> amount;

    // Add the transaction to the blockchain
    Transaction tx(sender, receiver, amount);
    Block newBlock(blockchain.chain.size(), {tx},
                   blockchain.getLastBlock().hash);
    blockchain.addBlock(newBlock);

    // Send the updated blockchain to all connected clients
    std::string chainString = blockchain.toString();
    for (tcp::socket& client : connectedClients) {
      client.write_some(boost::asio::buffer(chainString));
    }

    // Close the socket
    socket.close();
  } catch (std::exception& e) {
    std::cerr << "Exception in thread: " << e.what() << std::endl;
  }
}

int main() {
  try {
    // Create a blockchain instance
    Blockchain blockchain = Blockchain(4);

    // Create an io_context object
    boost::asio::io_context io_context;

    // Create a tcp::acceptor object to listen for incoming connections
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));

    // Start accepting incoming connections
    while (true) {
      tcp::socket socket(io_context);
      acceptor.accept(socket);

      // Add the client socket to the vector of connected clients
      connectedClients.push_back(std::move(socket));

      // Start a new thread to handle the incoming connection
      std::thread(handleConnection, std::move(socket), std::ref(blockchain))
          .detach();
    }
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}
