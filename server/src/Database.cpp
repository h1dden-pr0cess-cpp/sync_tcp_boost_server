#include "Database.hpp"
#include <iostream>

Database::Database(const std::string& filename)
{
    if (sqlite3_open(filename.c_str(), &db_) != SQLITE_OK)
    {
        std::cerr << "Cannot open database\n";
        db_ = nullptr;
    }

    create_tables();
}

Database::~Database()
{
    if (db_)
        sqlite3_close(db_);
}

void Database::create_tables()
{
    const char* sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE NOT NULL,"
        "password_hash TEXT NOT NULL"
        ");";

    char* err = nullptr;

    if (sqlite3_exec(db_, sql, nullptr, nullptr, &err) != SQLITE_OK)
    {
        std::cerr << "Table creation failed: " << err << "\n";
        sqlite3_free(err);
    }
}

bool Database::add_user(const std::string& username,
                        const std::string& password_hash)
{
    const char* sql =
        "INSERT INTO users(username, password_hash) VALUES(?, ?);";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password_hash.c_str(), -1, SQLITE_STATIC);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);

    return success;
}

bool Database::check_user(const std::string& username,
                          const std::string& password_hash)
{
    const char* sql =
        "SELECT password_hash FROM users WHERE username = ?;";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    bool result = false;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const unsigned char* stored =
            sqlite3_column_text(stmt, 0);

        if (stored &&
            password_hash == reinterpret_cast<const char*>(stored))
        {
            result = true;
        }
    }

    sqlite3_finalize(stmt);
    return result;
}

bool Database::user_exists(const std::string& username)
{
    const char* sql =
        "SELECT 1 FROM users WHERE username = ? LIMIT 1;";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);

    sqlite3_finalize(stmt);

    return exists;
}
