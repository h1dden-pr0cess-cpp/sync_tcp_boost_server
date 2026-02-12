#ifndef SESSION_HPP
#define SESSION_HPP

#include "Protocol.hpp"
#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <cstdint>
#include <iostream>
#include <cstring>
#include <stdexcept>

class Server;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(int id, boost::asio::ip::tcp::socket socket, Server& server);
		

    void start();
    int id() const; 

private:
    void read_header();
    void read_body();
    void handle_packet();
	void handle_register(const std::vector<uint8_t>& data);
	void handle_login_password(const std::vector<uint8_t>& data);
	void handle_login_token(const std::vector<uint8_t>& data);
	void handle_logout();
	void send_packet(const Packet& packet);

private:

	Server& server_;

	static constexpr uint32_t MAX_PACKET_SIZE = 10 * 1024 * 1024;

	bool authorized_ = false;
    std::string username_;

    int id_;

    uint8_t packet_type_;
    uint32_t packet_size_;  
    std::vector<uint8_t> body_; 

    boost::asio::ip::tcp::socket socket_;
};

#endif // SESSION_HPP

