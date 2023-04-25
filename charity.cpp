#include <sqlite3.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using namespace std;
using json = nlohmann::json;

class Filial {
 public:
  Filial(sqlite3* db) : m_db(db) {
    const std::string sql =
        "CREATE TABLE IF NOT EXISTS filials ("
        "filial_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "city TEXT NOT NULL,"
        "street TEXT NOT NULL,"
        "house TEXT NOT NULL,"
        "phone_number TEXT NOT NULL);";
    char* errorMsg = nullptr;
    if (sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errorMsg) !=
        SQLITE_OK) {
      std::cerr << "Error creating filials table: " << errorMsg << std::endl;
      sqlite3_free(errorMsg);
    }
  }

 private:
  sqlite3* m_db;
};

class Worker {
 public:
  Worker(sqlite3* db) : m_db(db) {
    const std::string sql =
        "CREATE TABLE IF NOT EXISTS workers ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "first_name TEXT NOT NULL,"
        "second_name TEXT NOT NULL,"
        "third_name TEXT NOT NULL,"
        "phone_number TEXT NOT NULL,"
        "role TEXT NOT NULL,"
        "filial_id INT NOT NULL,"
        "FOREIGN KEY(filial_id) REFERENCES filials(filial_id));";
    char* errorMsg = nullptr;
    if (sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errorMsg) !=
        SQLITE_OK) {
      std::cerr << "Error creating workers table: " << errorMsg << std::endl;
      sqlite3_free(errorMsg);
    }
  }

 private:
  sqlite3* m_db;
};

class Help {
 public:
  Help(sqlite3* db) : m_db(db) {
    const std::string sql =
        "CREATE TABLE IF NOT EXISTS help ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "first_name TEXT NOT NULL,"
        "second_name TEXT NOT NULL,"
        "third_name TEXT NOT NULL,"
        "problem TEXT NOT NULL,"
        "money_goal INT NOT NULL,"
        "filial_id INT NOT NULL,"
        "FOREIGN KEY(filial_id) REFERENCES filials(filial_id));";
    char* errorMsg = nullptr;
    if (sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errorMsg) !=
        SQLITE_OK) {
      std::cerr << "Error creating help table: " << errorMsg << std::endl;
      sqlite3_free(errorMsg);
    }
  }

  void createHelp(const std::string& firstName, const std::string& secondName,
                  const std::string& thirdName, const std::string& problem,
                  int moneyGoal, int filialId) {
    const std::string sql =
        "INSERT INTO help (first_name, second_name, third_name, problem, "
        "money_goal, filial_id) "
        "VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) !=
        SQLITE_OK) {
      std::cerr << "Error preparing create help statement: "
                << sqlite3_errmsg(m_db) << std::endl;
      return;
    }

    sqlite3_bind_text(stmt, 1, firstName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, secondName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, thirdName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, problem.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, moneyGoal);
    sqlite3_bind_int(stmt, 6, filialId);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
      std::cerr << "Error executing create help statement: "
                << sqlite3_errmsg(m_db) << std::endl;
      sqlite3_finalize(stmt);
      return;
    }

    sqlite3_finalize(stmt);
  }
  std::string readHelp() {
    const std::string sql = "SELECT * FROM help;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) !=
        SQLITE_OK) {
      std::cerr << "Error preparing statement: " << sqlite3_errmsg(m_db)
                << std::endl;
      return "";
    }

    nlohmann::json result;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      int id = sqlite3_column_int(stmt, 0);
      std::string first_name =
          reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
      std::string second_name =
          reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
      std::string third_name =
          reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
      std::string problem =
          reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
      int money_goal = sqlite3_column_int(stmt, 5);
      int filial_id = sqlite3_column_int(stmt, 6);

      nlohmann::json row;
      row["id"] = id;
      row["first_name"] = first_name;
      row["second_name"] = second_name;
      row["third_name"] = third_name;
      row["problem"] = problem;
      row["money_goal"] = money_goal;
      row["filial_id"] = filial_id;

      result.push_back(row);
    }

    sqlite3_finalize(stmt);

    return result.dump();
  }

  std::string readHelpByID(int id) {
    std::string sql = "SELECT * FROM help WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) !=
        SQLITE_OK) {
      std::cerr << "Error preparing statement: " << sqlite3_errmsg(m_db)
                << std::endl;
      return "";
    }

    // Bind the id parameter to the prepared statement
    if (sqlite3_bind_int(stmt, 1, id) != SQLITE_OK) {
      std::cerr << "Error binding parameter: " << sqlite3_errmsg(m_db)
                << std::endl;
      sqlite3_finalize(stmt);
      return "";
    }

    nlohmann::json result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
      // Read the row data into variables
      std::string first_name =
          reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
      std::string second_name =
          reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
      std::string third_name =
          reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
      std::string problem =
          reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
      int money_goal = sqlite3_column_int(stmt, 5);
      int filial_id = sqlite3_column_int(stmt, 6);

      // Create a JSON object for the row
      nlohmann::json row;
      row["id"] = id;
      row["first_name"] = first_name;
      row["second_name"] = second_name;
      row["third_name"] = third_name;
      row["problem"] = problem;
      row["money_goal"] = money_goal;
      row["filial_id"] = filial_id;

      // Add the row to the result array
      result.push_back(row);
    }

    sqlite3_finalize(stmt);

    return result.dump();
  }

  bool updateById(int id, const std::string& first_name,
                  const std::string& second_name, const std::string& third_name,
                  const std::string& problem, int money_goal, int filial_id) {
    const std::string sql =
        "UPDATE help SET first_name = ?, second_name = ?, third_name = ?, "
        "problem = ?, "
        "money_goal = ?, filial_id = ? WHERE id = ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) !=
        SQLITE_OK) {
      std::cerr << "Error preparing statement: " << sqlite3_errmsg(m_db)
                << std::endl;
      return false;
    }

    sqlite3_bind_text(stmt, 1, first_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, second_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, third_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, problem.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, money_goal);
    sqlite3_bind_int(stmt, 6, filial_id);
    sqlite3_bind_int(stmt, 7, id);

    int result = sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    return result == SQLITE_DONE;
  }

  void deleteHelp(int helpId) {
    const std::string sql = "DELETE FROM help WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) !=
        SQLITE_OK) {
      std::cerr << "Error preparing delete help statement: "
                << sqlite3_errmsg(m_db) << std::endl;
      return;
    }

    sqlite3_bind_int(stmt, 1, helpId);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
      std::cerr << "Error executing delete help statement: "
                << sqlite3_errmsg(m_db) << std::endl;
      sqlite3_finalize(stmt);
      return;
    }

    sqlite3_finalize(stmt);
  }

 private:
  sqlite3* m_db;
};

class Volunteer {
 public:
  Volunteer(sqlite3* db) : m_db(db) {
    const std::string sql =
        "CREATE TABLE IF NOT EXISTS volunteers ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "first_name TEXT NOT NULL,"
        "second_name TEXT NOT NULL,"
        "third_name TEXT NOT NULL,"
        "filial_id INT NOT NULL,"
        "phone_number TEXT NOT NULL,"
        "email TEXT NOT NULL,"
        "FOREIGN KEY(filial_id) REFERENCES filials(filial_id));";
    char* errorMsg = nullptr;
    if (sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errorMsg) !=
        SQLITE_OK) {
      std::cerr << "Error creating volunteers table: " << errorMsg << std::endl;
      sqlite3_free(errorMsg);
    }
  }

 private:
  sqlite3* m_db;
};

class Contractor {
 public:
  Contractor(sqlite3* db) : m_db(db) {
    const std::string sql =
        "CREATE TABLE IF NOT EXISTS contractors ("
        "contractor_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "description TEXT NOT NULL,"
        "phone_number TEXT NOT NULL,"
        "email TEXT NOT NULL,"
        "filial_id INT NOT NULL,"
        "FOREIGN KEY(filial_id) REFERENCES filials(filial_id));";
    char* errorMsg = nullptr;
    if (sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errorMsg) !=
        SQLITE_OK) {
      std::cerr << "Error creating contractors table: " << errorMsg
                << std::endl;
      sqlite3_free(errorMsg);
    }
  }

 private:
  sqlite3* m_db;
};

class Contract {
 public:
  Contract(sqlite3* db) : m_db(db) {
    const std::string sql =
        "CREATE TABLE IF NOT EXISTS contracts ("
        "contractor_id INT NOT NULL,"
        "filial_id INT NOT NULL,"
        "description TEXT NOT NULL,"
        "start_date DATE NOT NULL,"
        "finish_date DATE NOT NULL,"
        "FOREIGN KEY(contractor_id) REFERENCES contractors(id),"
        "FOREIGN KEY(filial_id) REFERENCES filials(filial_id));";
    char* errorMsg = nullptr;
    if (sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errorMsg) !=
        SQLITE_OK) {
      std::cerr << "Error creating contracts table: " << errorMsg << std::endl;
      sqlite3_free(errorMsg);
    }
  }

 private:
  sqlite3* m_db;
};

// int main() {
//   sqlite3* db;
//   if (sqlite3_open("charity.db", &db) != SQLITE_OK) {
//     std::cerr << "Error opening database: " << sqlite3_errmsg(db) <<
//     std::endl; return 1;
//   }
//   Filial filial(db);
//   Worker worker(db);
//   Help help(db);
//   help.createHelp("sdfs", "", "", "", 12, 1);
//   Volunteer volunteer(db);
//   Contractor contractor(db);
//   Contract contract(db);
//   sqlite3_close(db);

//   return 0;
// }