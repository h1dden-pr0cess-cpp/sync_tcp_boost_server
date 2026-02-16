#ifndef SERVER_HPP
#define SERVER_HPP

#include "Database.hpp"
#include <boost/asio.hpp>
#include <string>

struct User {
    std::string username;
    std::string password_hash;
};


class Server
{
public:
    Server(boost::asio::io_context& io, unsigned short port, Database& db);

	void add_user(const User& user);
    bool check_user(const std::string& username, const std::string& password_hash);
    bool user_exists(const std::string& username);

private:
	Database& db_;
	boost::asio::ip::tcp::acceptor acceptor_;
	int next_id_;

    void do_accept();  
};

#endif // SERVER_HPP

