#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <string>
#include <cstdint>
#include <vector>


enum class PacketType : uint8_t
{
    Ping             = 1,
	UploadStart      = 2,
    UploadChunk      = 3,
	UploadEnd        = 4,

	AddGame			 = 10,
	DeleteGame	     = 11,
	ListGames	     = 12,
	AddSave          = 13,
	DeleteSave       = 14,
	ListSaves        = 15,
    DownloadSave     = 16,
	
    RegisterUser     = 20, 
    LoginWithPassword= 21, 
    LoginWithToken   = 22, 
    Logout           = 23,

	AuthResponse     = 30,
    Error            = 99
};


struct Packet
{
    PacketType type;
    std::vector<uint8_t> body;
};

struct UploadState{
	std::string filename;
	std::vector<uint8_t> data;
	bool active = false;
};


struct AuthPasswordPacket
{
    std::string username;
    std::string password_hash; 
};

struct AuthTokenPacket
{
    std::string token; 
};

struct RegisterPacket
{
    std::string username;
    std::string password_hash;
};

struct AuthResponsePacket
{
    bool success;
    std::string message; 
	std::string token;  
};

#endif // PROTOCOL_HPP

