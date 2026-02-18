#ifndef SERVER_HPP
#define SERVER_HPP

#include "Database.hpp"
#include <unordered_map>
#include <boost/asio.hpp>
#include <string>
#include <boost/asio/ssl.hpp>

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

	std::string generate_token(const std::string& username);

	bool validate_token(const std::string& token, std::string& username);

	void remove_token(const std::string& token);

	boost::asio::ssl::context& get_ssl_context() {
        return ssl_context_;
    }

private:
	Database& db_;
	boost::asio::ip::tcp::acceptor acceptor_;
	int next_id_;
	boost::asio::ssl::context ssl_context_;
	
	std::unordered_map<std::string, std::string> tokens_;
	std::mutex tokens_mutex_;

    void do_accept();  


};

#endif // SERVER_HPP

