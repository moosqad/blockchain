#include <openssl/sha.h>
#include <sqlite3.h>

#include <atomic>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using namespace std;
using json = nlohmann::json;

// Функция вычисляющая SHA-256 хеш строки
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
// Структура транзакции блокчейна
struct Transaction {
  string sender;
  string receiver;
  float amount;

  Transaction(string s, string r, float a) {
    sender = s;
    receiver = r;
    amount = a;
  }
};

// Структура блока в блокчейне
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
    hash = calculate_hash();
    timestamp = time(0);
  }

  string calculate_hash() {
    string data = to_string(index) + previousHash + to_string(timestamp) +
                  to_string(nonce);
    for (auto tx : transactions) {
      data += tx.sender + tx.receiver + to_string(tx.amount);
    }
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)data.c_str(), data.length(), hash);
    string hash_str = "";
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
      char hex[3];
      snprintf(hex, 3, "%02x", hash[i]);
      hash_str += hex;
    }
    return hash_str;
  }

  // функция вычисления хеша блока
  string calculateHash() const {
    stringstream ss;
    ss << index << timestamp << previousHash << nonce;
    for (Transaction tx : transactions) {
      ss << tx.sender << tx.receiver << tx.amount;
    }
    return sha256(ss.str());
  }
  string calculateHashWithNonce(int nonce) const {
    stringstream ss;
    ss << index << timestamp << previousHash << nonce;
    for (const Transaction& tx : transactions) {
      ss << tx.sender << tx.receiver << tx.amount;
    }
    string data = ss.str();

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.c_str(), data.size());
    SHA256_Final(hash, &sha256);

    stringstream result;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
      result << hex << setw(2) << setfill('0') << (int)hash[i];
    }

    return result.str();
  }

  void mineBlock(int difficulty, int numThreads) {
    atomic<bool> blockMined(false);
    mutex blockMutex;

    vector<thread> threads;

    for (int i = 0; i < numThreads; i++) {
      threads.emplace_back([difficulty, this, &blockMined, &blockMutex]() {
        int localNonce = 0;
        string localHash;

        while (!blockMined) {
          localHash = calculateHashWithNonce(localNonce);

          // Check if the block is mined by comparing the prefix of the hash
          // with the desired difficulty
          if (localHash.substr(0, difficulty) == string(difficulty, '0')) {
            {
              lock_guard<mutex> lock(blockMutex);
              if (!blockMined) {
                blockMined = true;
                nonce = localNonce;
                hash = localHash;
              }
            }
            break;
          }

          localNonce++;
        }
      });
    }

    for (auto& thread : threads) {
      thread.join();
    }

    cout << "Block mined: " << hash << endl;
  }

  // void mineBlock(int difficulty, int numThreads) {
  //   cout << numThreads;
  //   while (hash.substr(0, difficulty) != string(difficulty, '0')) {
  //     nonce++;
  //     hash = calculate_hash();
  //   }
  //   cout << "Block mined: " << hash << endl;
  // }
  // Добавление транзакции в блокчейн, хранящийся в оперативной памяти
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

// Структура блокчейна
class Blockchain {
  //  private:

 public:
  sqlite3* db;
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
        "sender TEXT, receiver TEXT, amount REAL, block_index INTEGER)";
    rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
      cerr << "Error creating transactions table: " << errmsg << endl;
    }
  }
  // Function to get the sum of every transaction for a specific receiver
  double getReceiverTotal(string receiver) {
    string sql = "SELECT sum(amount) FROM transactions WHERE receiver='" +
                 receiver + "'";
    sqlite3_stmt* stmt;
    double total = 0;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
      cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
      return total;
    }
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
      total = sqlite3_column_double(stmt, 0);
    } else {
      cerr << "Error getting receiver total: " << sqlite3_errmsg(db) << endl;
    }
    sqlite3_finalize(stmt);
    return total;
  }

  // Функция создания первого (genesis) блока
  Block createGenesisBlock() {
    vector<Transaction> txs;
    Transaction tx = Transaction("0000", "0000", 0);
    txs.push_back(tx);
    return Block(0, txs, "0");
  }

  // Function to get the last block in the chain
  Block getLastBlock() { return chain.back(); }
  string toString();
  bool isValid(sqlite3* db);

  // функция добавляющая блок в цепочку блокчейна
  void addBlock(Block newBlock) {
    newBlock.previousHash = getLastBlock().hash;
    newBlock.mineBlock(difficulty, 2);
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
        "block_index) VALUES (?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
      cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
      return;
    }
    sqlite3_bind_text(stmt, 1, tx.sender.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, tx.receiver.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, (double)tx.amount);
    sqlite3_bind_int(stmt, 4, block_index);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
      cerr << "Error adding transaction: " << sqlite3_errmsg(db) << endl;
    }
    sqlite3_finalize(stmt);
  }

  json toJSON() const {
    json blockchain;
    for (const Block& block : chain) {
      json b;
      b["index"] = block.index;
      b["timestamp"] = block.timestamp;
      b["nonce"] = block.nonce;
      b["previous_hash"] = block.previousHash;
      b["hash"] = block.hash;

      json txs;
      for (const Transaction& tx : block.transactions) {
        json t;
        t["sender"] = tx.sender;
        t["receiver"] = tx.receiver;
        t["amount"] = tx.amount;
        txs.push_back(t);
      }
      b["transactions"] = txs;

      blockchain.push_back(b);
    }
    return blockchain;
  }
};

// перевод цепи блокчейна в строковое представление
std::string Blockchain::toString() {
  std::stringstream ss;
  for (const Block& block : chain) {
    ss << block.toString() << std::endl;
  }
  return ss.str();
}
// функция проверки блокчейна на валидность
bool Blockchain::isValid(sqlite3* db) {
  Blockchain blockchain(1);
  string sql = "SELECT * FROM blocks ORDER BY bl_index";
  sqlite3_stmt* stmt;
  int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
    return false;
  }
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int index = sqlite3_column_int(stmt, 0);
    string hash = (const char*)sqlite3_column_text(stmt, 1);
    string previousHash = (const char*)sqlite3_column_text(stmt, 2);
    time_t timestamp = sqlite3_column_int(stmt, 3);
    int nonce = sqlite3_column_int(stmt, 4);
    vector<Transaction> transactions;
    string sql_tx = "SELECT * FROM transactions WHERE block_index = ?";
    sqlite3_stmt* stmt_tx;
    int rc_tx = sqlite3_prepare_v2(db, sql_tx.c_str(), -1, &stmt_tx, NULL);
    if (rc_tx != SQLITE_OK) {
      cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
      return false;
    }
    sqlite3_bind_int(stmt_tx, 1, index);
    while (sqlite3_step(stmt_tx) == SQLITE_ROW) {
      string sender = (const char*)sqlite3_column_text(stmt_tx, 1);
      string receiver = (const char*)sqlite3_column_text(stmt_tx, 2);
      float amount = sqlite3_column_double(stmt_tx, 3);

      transactions.push_back(Transaction(sender, receiver, amount));
    }
    sqlite3_finalize(stmt_tx);
    Block block(index, transactions, previousHash);
    block.timestamp = timestamp;
    block.nonce = nonce;
    block.hash = hash;
    blockchain.chain.push_back(block);
  }
  sqlite3_finalize(stmt);

  // теперь сравниваем блокчейн который хранится у нас и тот который мы создали
  // внутри функции
  if (blockchain.chain.size() !=
      (unsigned long)blockchain.getLastBlock().index + 1) {
    cerr << "Stored blockchain size is not equal to the actual blockchain size"
         << endl;
    return false;
  }
  for (unsigned long i = 1; i < blockchain.chain.size(); i++) {
    Block block = blockchain.chain[i];
    Block previousBlock = blockchain.chain[i - 1];
    if (block.previousHash != previousBlock.hash && i != 1) {
      cerr << "Block " << i << " has an invalid previous hash" << endl;
      return false;
    }
    if (block.calculate_hash() != block.hash) {
      cerr << "Block " << i << " has an invalid hash" << endl;
      return false;
    }
  }

  return true;
}

// Blockchain blockchain = Blockchain(4);  // set difficulty to 4
// int main() {
//   // add some transactions and blocks
//   blockchain.addBlock(Block(1, {Transaction("Alice", "Bob", 10.0)}, ""));
//   blockchain.addBlock(Block(2, {Transaction("Bob", "Charlie", 5.0)}, ""));
//   blockchain.addBlock(Block(3, {Transaction("Charlie", "Alice", 3.0)}, ""));

//   // print the blockchain
//   std::cout << "Blockchain:\n"
//             << blockchain.isValid(blockchain.db) << std::endl;

//   return 0;
// }
