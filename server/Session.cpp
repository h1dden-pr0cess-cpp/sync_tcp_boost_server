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
			boost::asio::buffer(body_),
			[self](const boost::system::error_code ec, std::size_t)
			{
				if(ec) return;
				
				self->handle_packet();
				self->read_size();
			}
	);
}

void Session::read_size()
{
	auto self = shared_from_this();

	boost::asio::async_read(
			socket_,
			boost::asio::buffer(&packet_size_, sizeof(packet_size_)),
			[self](const boost::system::error_code ec, std::size_t)
			{
				if(ec) return;
				
				self->body_.resize(self->packet_size_);
				self->read_body();
			}
	);
}
void Session::handle_packet()
{
	PacketType type = static_cast<PacketType>(body_[0]);

	switch(type)
	{
		case PacketType::Ping:
			std::cout << "Ping from client " << id_ << "\n";
			break;
		case PacketType::UploadChunk:
			std::cout << "Upload chunk from client " id_ << "\n";
			break;
		default:
			std::cout<< "Unknown packet from client " >> id_ << "\n";
			break;
	}
}
