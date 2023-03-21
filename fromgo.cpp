#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <sqlite3.h>

#include <bitset>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "sqlite3.h"
using namespace std;
namespace fs = std::filesystem;
using namespace fs;

typedef struct {
  sqlite3*
      db;  // то где хранится блокчейн, вместо хранения в оперативной памяти
  uint64_t index;  // для получения баланса, до конкретного блока (обращение к
  // конкретному состоянию блокчейна)
} BlockChain;

// typedef struct {
//   vector<uint8_t> RandBytes;
//   vector<uint8_t> PrevBlock;
//   string Sender;
//   string Receiver;
//   uint64_t Value;  // количество переданных средств
//   uint64_t ToStorage;  // налог хранилища блокчейна за проведенную транзакцию
//   vector<uint8_t> CurrHash;
//   uint8_t Signature[];
// } Transaction;

// typedef struct {  // блокчейн это усовершенствованная версия односвязного
// списка
//   vector<uint8_t> CurrHash;  // текущий хеш
//   vector<uint8_t> PrevHash;  // предыдущий хеш
//   uint64_t Nonce;
//   uint8_t Difficulty;
//   string Miner;               // адрес создателя блока
//   vector<uint8_t> Signature;  // подпис блока
//   string TimeStamp;           // время создания блока
//   vector<Transaction> Transactions;
//   map<string, uint64_t>
//       Mapping;  // состояния каждого из пользователей, участников блока
// } Block;

typedef struct {
  vector<uint8_t> RandBytes;
  vector<uint8_t> PrevBlock;
  string Sender;
  string Receiver;
  uint64_t Value;  // количество переданных средств
  uint64_t ToStorage;  // налог хранилища блокчейна за проведенную транзакцию
  vector<uint8_t> CurrHash;
  uint8_t Signature[];
} Transaction;

struct Block {
  uint64_t Nonce;
  uint8_t Difficulty;
  char CurrHash[1000];  // текущий хеш
  char PrevHash[1000];  // предыдущий хеш
  std::vector<Transaction> Transactions;
  std::map<std::string, uint64_t> Mapping;
  std::string Miner;
  std::string Signature;
  std::string Timestamp;
};

// struct BlockChain {
//   sqlite3* db;
// };

typedef struct {
  std::unique_ptr<RSA> PrivateKey;
} User;

// len(base64(sha256(data))) = 44

const string CREATE_TABLE =
    "CREATE TABLE BlockChain(Id INTEGER PRIMARY KEY AUTOINCREMENT, Hash "
    "VARCHAR(44) UNIQUE, Block TEXT);";
// #define CREATE_TABLE                                                    \
//   "CREATE TABLE BlockChain(Id INTEGER PRIMARY KEY AUTOINCREMENT, Hash " \
//   "VARCHAR(44) UNIQUE, Block TEXT);"

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

// func init() {
// 	mrand.Seed(time.Now().UnixNano())
// }

static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
  return 0;
}

int NewChain(string filename, string receiver) {
  std::ofstream file(filename);
  file.close();
  sqlite3* db;
  int rc = sqlite3_open(filename.c_str(), &db);
  if (rc != SQLITE_OK) {
    std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_close(db);
    return 1;
  }
  char* error;
  rc = sqlite3_exec(db, CREATE_TABLE.c_str(), callback, 0, &error);
  if (rc != SQLITE_OK) {
    std::cerr << "Failed to create table: " << error << std::endl;
    sqlite3_free(error);
    sqlite3_close(db);
    return 1;
  }
  BlockChain chain = {.db = db};
  Block genesis = {
      .PrevHash = GENESIS_BLOCK,
      .Mapping = {{STORAGE_CHAIN, STORAGE_VALUE}, {receiver, GENESIS_REWARD}},
      .Miner = receiver,
      .Timestamp = std::to_string(std::time(nullptr))};
  chain.AddBlock(genesis);
  sqlite3_close(db);
  return 0;
}

int main() {
  NewChain("chain.db", "receiver");
  return 0;
}
