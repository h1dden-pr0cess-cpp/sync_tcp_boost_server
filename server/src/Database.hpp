#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
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

private:
    sqlite3* db_;
    void create_tables();
};

#endif //DATABASE_HPP
