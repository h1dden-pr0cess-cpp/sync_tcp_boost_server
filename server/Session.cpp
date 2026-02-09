#include "Session.hpp"
#include <iostream>

Session::Session(int id, boost::asio::ip::tcp::socket socket)
    : id_(id),
      socket_(std::move(socket))
{}

int Session::id() const {
    return id_;
}

void Session::start() {
    read_header();
}


void Session::read_header() {
    auto self = shared_from_this();

    boost::asio::async_read(
        socket_,
        boost::asio::buffer(&packet_type_, sizeof(packet_type_)),
        [self](boost::system::error_code ec, std::size_t) {
            if (ec) return;

            boost::asio::async_read(
                self->socket_,
                boost::asio::buffer(&self->packet_size_, sizeof(self->packet_size_)),
                [self](boost::system::error_code ec, std::size_t) {
                    if (ec) return;

                    self->body_.resize(self->packet_size_);
                    self->read_body();
                }
            );
        }
    );
}


void Session::read_body() {
    auto self = shared_from_this();

    if (packet_size_ == 0) {
        handle_packet();
        read_header();
        return;
    }

    boost::asio::async_read(
        socket_,
		boost::asio::buffer(body_),
		[self](boost::system::error_code ec, std::size_t) {
            if (ec) return;

            self->handle_packet();
            self->read_header();
        }
    );
}


void Session::handle_packet() {
    PacketType type = static_cast<PacketType>(packet_type_);

    switch(type) {
        case PacketType::Ping:
            std::cout << "Ping from client " << id_ << "\n";
            break;
        case PacketType::UploadChunk:
            std::cout << "Upload chunk from client " << id_ << "\n";
            break;
        case PacketType::Download:
            std::cout << "Download request from client " << id_ << "\n";
            break;
        case PacketType::ListFiles:
            std::cout << "List files request from client " << id_ << "\n";
            break;
        default:
            std::cout << "Unknown packet from client " << id_ << "\n";
            break;
    }
}

