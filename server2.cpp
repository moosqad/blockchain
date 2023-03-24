#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

#include "blockchain.cpp"

using namespace std;

void handleConnection(int clientSocket, Blockchain& blockchain,
                      vector<int>& connectedClients) {
  char buffer[1024];
  int len = read(clientSocket, buffer, 1024);

  if (len <= 0) {
    close(clientSocket);
    return;
  }

  // Parse the incoming data to get the transaction details
  std::istringstream iss(buffer);
  std::string sender, receiver;
  float amount;
  iss >> sender >> receiver >> amount;

  // Add the transaction to the blockchain
  Transaction tx(sender, receiver, amount);
  Block newBlock(blockchain.chain.size(), {tx}, blockchain.getLastBlock().hash);
  blockchain.addBlock(newBlock);

  // Send the updated blockchain to all connected clients
  std::string chainString = blockchain.toString();
  for (int client : connectedClients) {
    if (client == clientSocket) {
      continue;
    }
    write(client, chainString.c_str(), chainString.length());
  }

  close(clientSocket);
}

int main() {
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    cerr << "Failed to create socket." << endl;
    return 1;
  }

  sockaddr_in serverAddress;
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(8080);

  // if (bind(serverSocket, (struct sockaddr*)&serverAddress,
  // sizeof(serverAddress)) == -1) {
  //   cerr << "Failed to bind socket." << endl;
  //   return 1;
  // }

  if (listen(serverSocket, SOMAXCONN) == -1) {
    cerr << "Failed to listen on socket." << endl;
    return 1;
  }

  Blockchain blockchain = Blockchain(4);
  vector<int> connectedClients;

  while (true) {
    sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);

    int clientSocket =
        accept(serverSocket, (sockaddr*)&clientAddress, &clientAddressSize);
    if (clientSocket == -1) {
      cerr << "Failed to accept incoming connection." << endl;
      continue;
    }

    connectedClients.push_back(clientSocket);

    std::thread(handleConnection, clientSocket, std::ref(blockchain),
                std::ref(connectedClients))
        .detach();
  }

  close(serverSocket);

  return 0;
}
