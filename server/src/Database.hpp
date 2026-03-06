#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <vector>
#include <mutex>
#include <sqlite3.h>

class Database
{
public:
    Database(const std::string& filename);
    ~Database();

    bool add_user(const std::string& username,
                  const std::string& password_hash);

    bool check_user(const std::string& username,
                    const std::string& password_hash);

    bool user_exists(const std::string& username);

	bool add_game(const std::string& username, const std::string& game_name);

	bool delete_game(const std::string& username, const std::string& game_name);

	std::vector<std::string>get_games(const std::string& username);

	bool add_save(const std::string& username, const std::string& game_name,
				  const std::string& save_name, const std::vector<uint8_t>& data);

	bool delete_save(const std::string& username, const std::string& game_name, const std::string& save_name);

	std::vector<std::string> get_saves(const std::string& username, const std::string& game_name);

private:
    sqlite3* db_;
	std::mutex mutex_;
    void create_tables();
};

#endif //DATABASE_HPP
