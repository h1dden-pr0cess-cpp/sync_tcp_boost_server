#include "Server.hpp"
#include "Session.hpp"
#include <iostream>
#include <random>
#include <sstream>
#include <iomanip>

Server::Server(boost::asio::io_context& io, unsigned short port, Database& db)
	: db_(db), 
	acceptor_(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
	next_id_(0),
	ssl_context_(boost::asio::ssl::context::tlsv12_server)
{
	ssl_context_.set_options(
        boost::asio::ssl::context::default_workarounds |
        boost::asio::ssl::context::no_sslv2 |
        boost::asio::ssl::context::no_sslv3 |
        boost::asio::ssl::context::single_dh_use);

	ssl_context_.use_certificate_chain_file("../certs/server.crt");
    ssl_context_.use_private_key_file("../certs/server.key", boost::asio::ssl::context::pem);

	do_accept();
}

void Server::do_accept()
{
    acceptor_.async_accept(
        [this](const boost::system::error_code& ec, boost::asio::ip::tcp::socket socket)
        {
            if (!ec)
            {
                int id = ++next_id_;
                std::cout << " Client " << id << " connected\n";

                std::make_shared<Session>(
                    id,
                    std::move(socket), // переносим сокет
                    *this             // передаём текущий сервер
                )->start();
            }

            do_accept();
        }
    );
}



void Server::add_user(const User& user) {
    db_.add_user(user.username, user.password_hash);
}

bool Server::check_user(const std::string& username, const std::string& password_hash) {
    return db_.check_user(username, password_hash);
}

bool Server::user_exists(const std::string& username) {
    return db_.user_exists(username);
}

std::string Server::generate_token(const std::string& username) {
    std::lock_guard<std::mutex> lock(tokens_mutex_);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    std::ostringstream ss;
    for (int i = 0; i < 16; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << dis(gen);
    }

    std::string token = ss.str();
    tokens_[token] = username;

    return token;
}

bool Server::validate_token(const std::string& token, std::string& out_username) {
    std::lock_guard<std::mutex> lock(tokens_mutex_);
    auto it = tokens_.find(token);
    if (it == tokens_.end())
        return false;

    out_username = it->second;
    return true;
}

void Server::remove_token(const std::string& token)
{
	tokens_.erase(token);
}

	

