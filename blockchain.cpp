#include <openssl/sha.h>
#include <sqlite3.h>

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// конкретная область для благотворительности
// структура, что-то через блокчейн, что такое блокчейн, (на примере утечки
// денег) обосновать что сделал и зачем

// Function to calculate the SHA-256 hash of a string
string sha256(string str) {
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, str.c_str(), str.size());
  SHA256_Final(hash, &sha256);

  stringstream ss;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    ss << hex << setw(2) << setfill('0') << (int)hash[i];
  }

  return ss.str();
}
// Define a transaction structure
struct Transaction {
  string sender;
  string receiver;
  float amount;
  string signature;

  Transaction(string s, string r, float a, string sign) {
    sender = s;
    receiver = r;
    amount = a;
    signature = sign;
  }
};

// Define a block structure
struct Block {
  int index;
  time_t timestamp;
  vector<Transaction> transactions;
  int nonce;
  string previousHash;
  string hash;

  Block(int idx, vector<Transaction> txs, string prevHash) {
    index = idx;
    transactions = txs;
    previousHash = prevHash;
    nonce = 0;
    hash = calculateHash();
    timestamp = time(0);
  }

  string calculate_hash() {
    string data = to_string(index) + previousHash + to_string(timestamp) +
                  to_string(nonce);
    for (auto tx : transactions) {
      data += tx.sender + tx.receiver + to_string(tx.amount) + tx.signature;
    }
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)data.c_str(), data.length(), hash);
    string hash_str = "";
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
      char hex[3];
      sprintf(hex, "%02x", hash[i]);
      hash_str += hex;
    }
    return hash_str;
  }

  // Function to calculate the hash of the block
  string calculateHash() const {
    stringstream ss;
    ss << index << timestamp << previousHash << nonce;
    for (Transaction tx : transactions) {
      ss << tx.sender << tx.receiver << tx.amount;
    }
    return sha256(ss.str());
  }

  // Function to mine the block
  void mineBlock(int difficulty) {
    while (hash.substr(0, difficulty) != string(difficulty, '0')) {
      nonce++;
      hash = calculateHash();
    }
    cout << "Block mined: " << hash << endl;
  }

  // Function to add a transaction to the block
  void addTransaction(Transaction tx) { transactions.push_back(tx); }
  string toString() const {
    stringstream ss;
    ss << "Block #" << index << " (timestamp: " << timestamp << "):" << endl;
    ss << "Previous hash: " << previousHash << endl;
    ss << "Nonce: " << nonce << endl;
    ss << "Transactions: " << endl;
    for (const Transaction& tx : transactions) {
      ss << "  " << tx.sender << " -> " << tx.receiver << ": " << tx.amount
         << endl;
    }
    ss << "Hash: " << hash << endl;
    return ss.str();
  }
};

// Define a blockchain structure
class Blockchain {
 private:
  sqlite3* db;

 public:
  vector<Block> chain;
  int difficulty;

  Blockchain(int diff) {
    difficulty = diff;
    chain.push_back(createGenesisBlock());
    int rc = sqlite3_open("blockchain.db", &db);
    if (rc) {
      cerr << "Error opening database: " << sqlite3_errmsg(db);
    }
    string sql =
        "CREATE TABLE IF NOT EXISTS blocks (bl_index INTEGER PRIMARY KEY, "
        "hash "
        "TEXT, previousHash TEXT, timestamp INTEGER, nonce INTEGER)";
    char* errmsg;
    rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
      cerr << "Error creating blocks table: " << errmsg << endl;
    }
    sql =
        "CREATE TABLE IF NOT EXISTS transactions (id INTEGER PRIMARY KEY, "
        "sender TEXT, receiver TEXT, amount REAL, signature "
        "TEXT, block_index INTEGER)";
    rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
      cerr << "Error creating transactions table: " << errmsg << endl;
    }
  }

  // Function to create the first block (genesis block)
  Block createGenesisBlock() {
    vector<Transaction> txs;
    Transaction tx = Transaction("0000", "0000", 0, "Genesis sign");
    txs.push_back(tx);
    return Block(0, txs, "0");
  }

  // Function to get the last block in the chain
  Block getLastBlock() { return chain.back(); }
  string toString();
  bool isValid();

  // Function to add a new block to the chain
  void addBlock(Block newBlock) {
    newBlock.previousHash = getLastBlock().hash;
    newBlock.mineBlock(difficulty);
    chain.push_back(newBlock);

    string sql =
        "INSERT INTO blocks (bl_index, hash, previousHash, timestamp, "
        "nonce) "
        "VALUES "
        "(?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
      cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
      return;
    }
    sqlite3_bind_int(stmt, 1, newBlock.index);
    sqlite3_bind_text(stmt, 2, newBlock.calculate_hash().c_str(), -1,
                      SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, newBlock.previousHash.c_str(), -1,
                      SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, newBlock.timestamp);
    sqlite3_bind_int(stmt, 5, newBlock.nonce);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
      cerr << "Error adding newBlock: " << sqlite3_errmsg(db) << endl;
    }
    sqlite3_finalize(stmt);
    for (auto tx : newBlock.transactions) {
      add_transaction(tx, newBlock.index);
    }
  }

  void add_transaction(Transaction tx, int block_index) {
    string sql =
        "INSERT INTO transactions (sender, receiver, amount, "
        "signature, block_index) VALUES (?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
      cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
      return;
    }
    sqlite3_bind_text(stmt, 1, tx.sender.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, tx.receiver.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, (double)tx.amount);
    sqlite3_bind_text(stmt, 4, tx.signature.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, block_index);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
      cerr << "Error adding transaction: " << sqlite3_errmsg(db) << endl;
    }
    sqlite3_finalize(stmt);
  }

  // Function to check if the chain is valid
  bool isChainValid() {
    for (int i = 1; i < chain.size(); i++) {
      Block currentBlock = chain[i];
      Block previousBlock = chain[i - 1];

      if (currentBlock.hash != currentBlock.calculateHash()) {
        cout << "Invalid hash for block " << currentBlock.index << endl;
        return false;
      }

      if (currentBlock.previousHash != previousBlock.hash) {
        cout << "Invalid previous hash for block " << currentBlock.index
             << endl;
        return false;
      }
    }
    return true;
  }
};

// Convert the blockchain to a string representation
std::string Blockchain::toString() {
  std::stringstream ss;
  for (const Block& block : chain) {
    ss << block.toString() << std::endl;
  }
  return ss.str();
}

// Check if the blockchain is valid
bool Blockchain::isValid() {
  for (int i = 1; i < chain.size(); i++) {
    const Block& currentBlock = chain[i];
    const Block& previousBlock = chain[i - 1];

    // Check if the current block's hash is valid
    if (currentBlock.hash != currentBlock.calculateHash()) {
      return false;
    }

    // Check if the current block points to the previous block
    if (currentBlock.previousHash != previousBlock.hash) {
      return false;
    }
  }
  return true;
}

int main() {
  Blockchain blockchain = Blockchain(4);  // set difficulty to 4

  // add some transactions and blocks
  blockchain.addBlock(
      Block(1, {Transaction("Alice", "Bob", 10.0, "Alice's sign")}, ""));
  blockchain.addBlock(
      Block(2, {Transaction("Bob", "Charlie", 5.0, "Bob's sign")}, ""));
  blockchain.addBlock(
      Block(3, {Transaction("Charlie", "Alice", 3.0, "Charlie's sign")}, ""));

  // print the blockchain
  std::cout << "Blockchain:\n" << blockchain.toString() << std::endl;

  // verify the blockchain
  if (blockchain.isValid()) {
    std::cout << "Blockchain is valid" << std::endl;
  } else {
    std::cout << "Blockchain is NOT valid" << std::endl;
  }

  return 0;
}