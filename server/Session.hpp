#ifndef SESSION_HPP
#define SESSION_HPP

#include "Protocol"
#include <boost/asio.hpp>
#include <memory>
#include <array>
#include <vector>

class Session : public std::enable_shared_from_this<Session>
{
public:
    void start();

private:
    void read_size();
    void read_body();
	void handle_packet();

    uint32_t current_size_;
    std::vector<uint8_t> body_;

    boost::asio::ip::tcp::socket socket_;
};


#endif // SESSION_HPP

