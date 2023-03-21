#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <sqlite3.h>

#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "sqlite3.h"
using namespace std;
namespace fs = std::filesystem;
using namespace fs;

typedef struct {
  sqlite3* db;
  uint64_t index;
} BlockChain;

typedef struct {
  uint8_t RandBytes;
  uint8_t PrevBlock;
  string Sender;
  string Receiver;
  uint64_t Value;
  uint64_t ToStorage;
  vector<uint8_t> CurrHash;
  uint8_t Signature[];
} Transaction;

typedef struct {
  vector<uint8_t> CurrHash;
  vector<uint8_t> PrevHash;
  uint64_t Nonce;
  uint8_t Difficulty;
  string Miner;
  vector<uint8_t> Signature;
  string TimeStamp;
  vector<Transaction> Transactions;
  map<string, uint64_t> Mapping;
} Block;

// typedef struct {} USER

const char* CREATE_TABLE =
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
  int i;
  for (i = 0; i < argc; i++) {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

void* NewChain(const string filename, string receiver) {
  int err = 0;
  if (!fs::create_directories(filename)) {
    cout << "CREATING FOLDER ERROR\n";
    return NULL;
  }
  sqlite3* db = 0;
  const char* file = filename.c_str();
  if (!sqlite3_open("test.db", &db)) {
    cout << "CREATING DB ERROR";
    sqlite3_close(db);
    // return NULL;
  }
  char* errMSG;

  if (sqlite3_exec(db, CREATE_TABLE, callback, 0, &errMSG) != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", errMSG);
    sqlite3_free(errMSG);
  } else {
    fprintf(stdout, "Table created successfully\n");
  }
  sqlite3_close(db);
  return NULL;
}

int main() {
  NewChain("HELLO", "H");
  return 0;
}
