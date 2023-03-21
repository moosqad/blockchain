#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <sqlite3.h>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#define CREATE_TABLE \
  "CREATE TABLE BlockChain ( \
	Id INTEGER PRIMARY KEY AUTOINCREMENT, \
	Hash VARCHAR(44) UNIQUE, \
	Block TEXT \
);"

#define KEY_SIZE 512
#define DEBUG true
#define TXS_LIMIT 2
#define DIFFICULTY 20
#define RAND_BYTES 32
#define STORAGE_REWARD 1
#define START_PERCENT 10

#define GENESIS_BLOCK "GENESIS-BLOCK"
#define STORAGE_VALUE 100
#define GENESIS_REWARD 100
#define STORAGE_CHAIN "STORAGE-CHAIN"

struct User {
  RSA* privateKey;
};

struct Transaction {
  vector<unsigned char> randBytes;
  vector<unsigned char> prevBlock;
  string sender;
  string receiver;
  uint64_t value;
  uint64_t toStorage;
  vector<unsigned char> currHash;
  vector<unsigned char> signature;
};

struct Block {
  vector<unsigned char> currHash;
  vector<unsigned char> prevHash;
  uint64_t nonce;
  uint8_t difficulty;
  string miner;
  vector<unsigned char> signature;
  string timeStamp;
  vector<Transaction> transactions;
  map<string, uint64_t> mapping;
};

class BlockChain {
 public:
  BlockChain() : index(0) {
    int rc = sqlite3_open("blockchain.db", &db);
    if (rc != SQLITE_OK) {
      cerr << "Error opening database: " << sqlite3_errmsg(db) << endl;
      sqlite3_close(db);
      exit(1);
    }
    char* errMsg = nullptr;
    rc = sqlite3_exec(db, CREATE_TABLE, NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
      cerr << "Error creating table: " << errMsg << endl;
      sqlite3_free(errMsg);
      sqlite3_close(db);
      exit(1);
    }
    sqlite3_free(errMsg);
  }

  void addBlock(Block& block) {
    ostringstream os;
    json j(block);
    os << j;
    string blockStr = os.str();

    vector<unsigned char> hash = sha256(blockStr);
    string hashStr = base64_encode(hash);

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(
        db, "INSERT INTO BlockChain (Hash, Block) VALUES (?, ?)", -1, &stmt,
        NULL);
    if (rc != SQLITE_OK) {
      cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
      sqlite3_close(db);
      exit(1);
    }
    rc = sqlite3_bind_text(stmt, 1, hashStr.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
      cerr << "Error binding parameter: " << sqlite3_errmsg(db) << endl;
      sqlite3_finalize(stmt);
      sqlite3_close(db);
      exit(1);
    }
    rc = sqlite3_bind_text(stmt, 2, blockStr.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
      cerr << "Error binding parameter: " << sqlite3_errmsg(db) << endl;
      sqlite3_finalize(stmt);
      sqlite3_close(db);
      exit(1);
    }
    rc
