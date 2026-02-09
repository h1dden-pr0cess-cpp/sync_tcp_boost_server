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
				if(!ec)
				{
					int id = ++next_id_;
					std::cout <<" Client " << id << " connected\n";

					std::make_shared<Session>(
							id,
							std::move(socket)
					)->start();
				}

				do_accept();
			}
		);
}
	

