#include "Session.hpp"
#include <iostream>

Session::Session(
    int id,
    boost::asio::ip::tcp::socket socket
)
    : id_(id),
      socket_(std::move(socket)){}

int Session::id() const {
    return id_;
}

void Session::start() {
    read_size();
}

void Session::read_body()
{
	auto self = shared_from_this();
	boost::asio::async_read(
			socket_,
			boost::asio::buffer(self->body_.data(), self->body_.size()),
			[self](const boost::system::error_code& ec, std::size_t)
			{
				if(ec){
					std::cout << "Client "
							  << self->id_
							  << " disconnected\n";
					return
				}
				std::cout << "Client "
						  << self->id_
						  << " sent "
						  << self->body_.size()
						  << " butes\n";

			}
}

void Session::read_size()
{
	auto self = shared_from_this();

	boost::asio::async_read();

	boost::asio::async_read(
			socket_,
			boost::asio::buffer(&current_size_, sizeof(current_size_)),
			[self](const boost::system::error_code& ec, std::size_t)
			{
				if(ec){ 
					std::cout << "Client "
							  << self->id_
							  << "disconnected\n";
					return;
				}
				self->body_.resize(self->current_size_);
				self->read_body();
			}
}
