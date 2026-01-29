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
    do_read();
}

void Session::do_read() {
    auto self = shared_from_this();
    auto buf = std::make_shared<std::array<char,1024>>();

    socket_.async_read_some(
        boost::asio::buffer(*buf),
        [self, buf](const boost::system::error_code& ec, std::size_t len)
        {
            if (ec) {
                std::cout << "Client "
                          << self->id_
                          << " disconnected\n";
                return;
            }

            std::cout << "Client "
                      << self->id_
                      << ": "
                      << std::string(buf->data(), len)
                      << std::endl;

            self->do_read();
        }
    );
}

