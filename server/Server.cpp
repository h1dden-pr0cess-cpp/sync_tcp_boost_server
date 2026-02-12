#include "Server.hpp"
#include "Session.hpp"
#include <iostream>

Server::Server(boost::asio::io_context& io, unsigned short port)
	: acceptor_(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)), next_id_(0) 
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

                // Здесь создаём сессию — вот именно это место нужно исправить:
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
    std::lock_guard<std::mutex> lock(users_mutex_);
    users_[user.username] = user; // если уже есть, заменит
}

bool Server::check_user(const std::string& username, const std::string& password_hash) {
    std::lock_guard<std::mutex> lock(users_mutex_);
    auto it = users_.find(username);
    return it != users_.end() && it->second.password_hash == password_hash;
}

bool Server::user_exists(const std::string& username) {
    std::lock_guard<std::mutex> lock(users_mutex_);
    return users_.find(username) != users_.end();
}

	

