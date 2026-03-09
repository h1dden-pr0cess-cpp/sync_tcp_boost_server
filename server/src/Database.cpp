#include "Database.hpp"
#include <iostream>

Database::Database(const std::string& filename)
{
    if (sqlite3_open(filename.c_str(), &db_) != SQLITE_OK)
    {
        std::cerr << "Cannot open database\n";
        db_ = nullptr;
        return;
    }

    // Включаем поддержку внешних ключей
    sqlite3_exec(db_, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    create_tables();
}

Database::~Database()
{
    if (db_)
        sqlite3_close(db_);
}

void Database::create_tables()
{
    const char* sql_users =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE NOT NULL,"
        "password_hash TEXT NOT NULL"
        ");";

    const char* sql_games =
        "CREATE TABLE IF NOT EXISTS games ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "game_name TEXT NOT NULL,"
        "FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE"
        ");";

    const char* sql_saves =
        "CREATE TABLE IF NOT EXISTS saves ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "game_id INTEGER NOT NULL,"
        "save_name TEXT NOT NULL,"
        "save_data BLOB,"
        "FOREIGN KEY(game_id) REFERENCES games(id) ON DELETE CASCADE"
        ");";

    char* err = nullptr;

    sqlite3_exec(db_, sql_users, nullptr, nullptr, &err);
    sqlite3_exec(db_, sql_games, nullptr, nullptr, &err);
    sqlite3_exec(db_, sql_saves, nullptr, nullptr, &err);

    if (err)
    {
        std::cerr << "Table creation failed: " << err << "\n";
        sqlite3_free(err);
    }
}

bool Database::add_user(const std::string& username,
                        const std::string& password_hash)
{
	std::lock_guard<std::mutex> lock(mutex_);

    const char* sql =
        "INSERT INTO users(username, password_hash) VALUES(?, ?);";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password_hash.c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    return success;
}

bool Database::check_user(const std::string& username,
                          const std::string& password_hash)
{
	std::lock_guard<std::mutex> lock(mutex_);

    const char* sql =
        "SELECT password_hash FROM users WHERE username = ?;";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

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
	std::lock_guard<std::mutex> lock(mutex_);

    const char* sql =
        "SELECT 1 FROM users WHERE username = ? LIMIT 1;";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);

    sqlite3_finalize(stmt);
    return exists;
}

bool Database::add_game(const std::string& username,
                        const std::string& game_name)
{
	std::lock_guard<std::mutex> lock(mutex_);

    const char* sql =
        "INSERT INTO games(user_id, game_name) "
        "SELECT id, ? FROM users WHERE username = ?;";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, game_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    return success;
}

bool Database::delete_game(const std::string& username,
                           const std::string& game_name)
{
	std::lock_guard<std::mutex> lock(mutex_);

    const char* sql =
        "DELETE FROM games "
        "WHERE id IN ("
        "   SELECT g.id FROM games g "
        "   JOIN users u ON g.user_id = u.id "
        "   WHERE u.username = ? AND g.game_name = ?"
        ");";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, game_name.c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    return success;
}

std::vector<std::string> Database::get_games(const std::string& username)
{
	std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::string> games;

    const char* sql =
        "SELECT g.game_name "
        "FROM games g "
        "JOIN users u ON g.user_id = u.id "
        "WHERE u.username = ?;";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const unsigned char* name =
            sqlite3_column_text(stmt, 0);

        if (name)
            games.emplace_back(reinterpret_cast<const char*>(name));
    }

    sqlite3_finalize(stmt);
    return games;
}

bool Database::add_save(const std::string& username,
                        const std::string& game_name,
                        const std::string& save_name,
                        const std::vector<uint8_t>& data)
{
	std::lock_guard<std::mutex> lock(mutex_);

    const char* sql =
        "INSERT INTO saves(game_id, save_name, save_data) "
        "SELECT g.id, ?, ? "
        "FROM games g "
        "JOIN users u ON g.user_id = u.id "
        "WHERE g.game_name = ? AND u.username = ?;";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, save_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_blob(stmt, 2, data.data(),
                      static_cast<int>(data.size()),
                      SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, game_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, username.c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    return success;
}

bool Database::delete_save(const std::string& username,
                           const std::string& game_name,
                           const std::string& save_name)
{
	std::lock_guard<std::mutex> lock(mutex_);

    const char* sql =
        "DELETE FROM saves "
        "WHERE id IN ("
        "   SELECT s.id FROM saves s "
        "   JOIN games g ON s.game_id = g.id "
        "   JOIN users u ON g.user_id = u.id "
        "   WHERE u.username = ? "
        "   AND g.game_name = ? "
        "   AND s.save_name = ?"
        ");";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, game_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, save_name.c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    return success;
}

std::vector<std::string> Database::get_saves(const std::string& username, const std::string& game_name)
{
	std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::string> saves;

    const char* sql =
        "SELECT s.save_name "
        "FROM saves s "
        "JOIN games g ON s.game_id = g.id "
        "JOIN users u ON g.user_id = u.id "
        "WHERE u.username = ? AND g.game_name = ?;";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, game_name.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const unsigned char* name =
            sqlite3_column_text(stmt, 0);

        if (name)
            saves.emplace_back(reinterpret_cast<const char*>(name));
    }

    sqlite3_finalize(stmt);
    return saves;
}

bool Database::get_save(const std::string& username,
                        const std::string& game_name,
                        const std::string& save_name,
                        std::vector<uint8_t>& save_out)
{
     save_out.clear();

    const char* sql = R"(
        SELECT save_data
        FROM saves
        WHERE username = ? AND game_name = ? AND save_name = ?
    )";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << "\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, game_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, save_name.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const void* blob_data = sqlite3_column_blob(stmt, 0);
        int blob_size = sqlite3_column_bytes(stmt, 0);

        if (blob_data && blob_size > 0) {
            const uint8_t* data_ptr = reinterpret_cast<const uint8_t*>(blob_data);
            save_out.assign(data_ptr, data_ptr + blob_size);
        }

        sqlite3_finalize(stmt);
        return true;
    } else if (rc == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return false;
    } else {
        std::cerr << "Error reading save: " << sqlite3_errmsg(db_) << "\n";
        sqlite3_finalize(stmt);
        return false;
    }
}


