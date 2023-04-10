#include "blockchain.cpp"

int main() {
  if (blockchain.isValid(blockchain.db)) {
    std::cout << "Blockchain is valid" << std::endl;
  } else {
    std::cout << "Blockchain is NOT valid" << std::endl;
  }
}