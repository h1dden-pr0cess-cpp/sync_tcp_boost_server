#ifndef SESSION_HPP
#define SESSION_HPP

#include "Protocol.hpp"
#include "Database.hpp"
#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <cstdint>
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <fstream>
#include <boost/asio/ssl.hpp>

class Server;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(int id, boost::asio::ip::tcp::socket socket, Server& server);
		

    void start();
    int id() const; 

private:
    void read_header();
    void read_body();

	std::string 
	read_one_string(const std::vector<uint8_t>& data); 
	std::pair<std::string, std::string>
	read_two_strings(const std::vector<uint8_t>& data);
	std::tuple<std::string, std::string, std::string> 
	read_three_strings(const std::vector<uint8_t>& data);
    

    void handle_packet();
	void handle_register(const std::vector<uint8_t>& data);
	void handle_login_password(const std::vector<uint8_t>& data);
	void handle_login_token(const std::vector<uint8_t>& data);
	void handle_upload_start(const std::vector<uint8_t>& data);
	void handle_upload_chunk(const std::vector<uint8_t>& data);
	void handle_upload_end(const std::vector<uint8_t>& data);
	void handle_logout();

	void handle_add_game(const std::string game_name);

	void handle_delete_game(const std::string game_name);

	void handle_list_games();

	void handle_add_save(const std::string game_name, 
						 const std::string save_name, 
					 	 const std::vector::<uint8_t> save);

	void handle_delete_save(const std::string game_name, 
							const std::string save_name);

	void handle_list_saves(const std::string game_name);

	void send_packet(const Packet& packet);
	void save_file();

	string get_username(){return username_;}

private:
	int id_;
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
	Server& server_;

	UploadState current_upload_;

	static constexpr uint32_t MAX_PACKET_SIZE = 10 * 1024 * 1024;

	bool authorized_ = false;
    std::string username_;

    uint8_t packet_type_;
    uint32_t packet_size_;  
    std::vector<uint8_t> body_; 
};

#endif // SESSION_HPP


