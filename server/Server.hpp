#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>
#include <unordered_map>
#include <mutex>
#include <string>

struct User {
    std::string username;
    std::string password_hash;
};


class Server
{
public:
    Server(boost::asio::io_context& io, unsigned short port);
	void add_user(const User& user);
    bool check_user(const std::string& username, const std::string& password_hash);
    bool user_exists(const std::string& username);

private:

	std::unordered_map<std::string, User> users_;
    std::mutex users_mutex_; // для безопасного доступа из разных сессий

    void do_accept();

    boost::asio::ip::tcp::acceptor acceptor_;
    int next_id_;
};

#endif // SERVER_HPP

