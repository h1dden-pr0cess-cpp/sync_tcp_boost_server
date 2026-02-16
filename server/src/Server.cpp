#include "Server.hpp"
#include "Session.hpp"
#include <iostream>

Server::Server(boost::asio::io_context& io, unsigned short port, Database& db)
	: db_(db), 
	acceptor_(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)), next_id_(0) 
{
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

	

