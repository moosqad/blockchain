#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {
  if (argc != 4) {
    cerr << "Usage: client <sender> <receiver> <amount>" << endl;
    return 1;
  }

  std::string sender = argv[1];
  std::string receiver = argv[2];
  float amount = std::stof(argv[3]);

  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket == -1) {
    cerr << "Failed to create socket." << endl;
    return 1;
  }

  sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  // serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverAddress.sin_port = htons(8080);

  if (connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) ==
      -1) {
    cerr << "Failed to connect to server." << endl;
    close(clientSocket);
    return 1;
  }

  // Send the transaction details to the server
  std::ostringstream oss;
  oss << sender << " " << receiver << " " << amount;
  std::string message = oss.str();
  write(clientSocket, message.c_str(), message.length());

  // Receive the updated blockchain from the server
  char buffer[1024];
  int len = read(clientSocket, buffer, 1024);

  if (len <= 0) {
    cerr << "Failed to receive blockchain from server." << endl;
    close(clientSocket);
    return 1;
  }

  std::string chainString(buffer, len);
  cout << "Received updated blockchain from server:\n" << chainString << endl;

  close(clientSocket);

  return 0;
}
