#include <sqlite3.h>

#include <iostream>
#include <string>

using namespace std;

class Users {
 public:
  string first_name;
  string second_name;
  string third_name;
  string phone_number;
  string email;
  string username;
  string password;
  float cash_amount;

  Users(string first_name, string second_name, string third_name,
        string phone_number, string email, string username, string password)
      : first_name(first_name),
        second_name(second_name),
        third_name(third_name),
        phone_number(phone_number),
        email(email),
        username(username),
        password(password),
        cash_amount(10000) {
    sqlite3_open("users.db", &db);
    sqlite3_exec(
        db,
        "CREATE TABLE IF NOT EXISTS users (username TEXT PRIMARY KEY, "
        "password TEXT, first_name TEXT, second_name TEXT, third_name "
        "TEXT, phone_number TEXT, email TEXT, cash_amount REAL);"
        "CREATE INDEX IF NOT EXISTS idx_username ON users (username ASC);",
        NULL, 0, NULL);
  }

  ~Users() { sqlite3_close(db); }
  void add_user() {
    string sql =
        "INSERT INTO users (username, password, first_name, second_name, "
        "third_name, phone_number, email, cash_amount) VALUES (?, ?, ?, ?, ?, "
        "?, ?, ?);";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, first_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, second_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, third_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, phone_number.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 8, cash_amount);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
  }

  bool user_exists() {
    string sql = "SELECT * FROM users WHERE username=?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return result == SQLITE_ROW;
  }

  bool is_valid_username_password() {
    string sql = "SELECT * FROM users WHERE username=? AND password=?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return result == SQLITE_ROW;
  }

  void update_cash_amount(float amount, string username) {
    cash_amount += amount;
    string sql = "UPDATE users SET cash_amount=? WHERE username=?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, cash_amount);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
  }

  int get_user_cash_amount() {
    string sql = "SELECT cash_amount FROM users WHERE username=?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    int result = sqlite3_step(stmt);
    int cash_amount = 0;
    if (result == SQLITE_ROW) {
      cash_amount = sqlite3_column_int(stmt, 0);
    } else {
      cout << "NO RECORDS WITH THIS USERNAME";
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return cash_amount;
  }

 private:
  sqlite3* db;
};

// int main() {
//   Users alice("Alice", "password1");
//   alice.add_user();
//   Users bob("Bob", "password2");
//   bob.add_user();

//   // cout << "Alice exists: " << alice.user_exists() << endl;
//   // cout << "Valid username/password: " <<
//   alice.is_valid_username_password()
//   //      << endl;

//   // cout << "Bob exists: " << bob.user_exists() << endl;
//   // cout << "Valid username/password: " << bob.is_valid_username_password()
//   //      << endl;

//   Users jack("Bodfb", "passwsdford2");
//   // if (!jack.user_exists()) jack.add_user();
//   // jack.add_user();

//   cout << "Jack exists: " << jack.user_exists() << endl;
//   cout << "Valid username/password: " << jack.is_valid_username_password()
//        << endl;

//   alice.update_cash_amount(500);
//   bob.update_cash_amount(-1000);

//   cout << "Alice cash amount: " << alice.cash_amount << endl;
//   cout << "Bob cash amount: " << bob.cash_amount << endl;

//   return 0;
// }
