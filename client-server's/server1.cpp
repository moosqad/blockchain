#include <boost/asio.hpp>
#include <iostream>
#include <pqxx/pqxx>  // Include the PostgreSQL library
#include <thread>
#include <vector>

#include "blockchain.cpp"

using boost::asio::ip::tcp;
std::vector<tcp::socket> connectedClients;

// Define a function to handle incoming client connections
void handleConnection(tcp::socket socket, pqxx::connection& dbConnection,
                      Blockchain& blockchain) {
  try {
    std::cout << "New client connected." << std::endl;

    // Define a buffer to hold incoming data
    char data[1024];

    // Read the incoming data from the socket
    boost::system::error_code error;
    size_t len = socket.read_some(boost::asio::buffer(data, 1024), error);

    // Check for errors and make sure the socket is still open
    if (error == boost::asio::error::eof) {
      std::cout << "Socket closed by client." << std::endl;
      return;
    } else if (error) {
      throw boost::system::system_error(error);
    }

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

    // Store the block in the database
    std::string sql =
        "INSERT INTO blocks (index, hash, previous_hash, timestamp, data) "
        "VALUES ($1, $2, $3, NOW(), $4)";
    dbConnection.prepare("insert_block", sql);
    pqxx::work txn(dbConnection);
    pqxx::result result =
        txn.prepared("insert_block")(newBlock.index)(newBlock.hash)(
               newBlock.previousHash)(newBlock.toString())
            .exec();

    // Commit the transaction to the database
    txn.commit();

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

    // Connect to the database
    pqxx::connection dbConnection(
        "dbname=mydb user=postgres password=postgres hostaddr=127.0.0.1 "
        "port=5432");

    // Create an io_context object
    boost::asio::io_context io_context;

    // Create a tcp::acceptor object to listen for incoming connections
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));

    // Start accepting incoming connections
    while (true) {
      tcp::socket socket(io_context);
      acceptor.accept(socket);

      // Start a new thread to handle the incoming connection
      std::thread(handleConnection, std::move(socket), std::ref(dbConnection),
                  std::ref(blockchain))
          .detach();
    }
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}
