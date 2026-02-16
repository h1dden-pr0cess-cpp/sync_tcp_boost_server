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
    Download         = 5,
    ListFiles        = 6,
	
    RegisterUser     = 10, 
    LoginWithPassword= 11, 
    LoginWithToken   = 12, 
    Logout           = 13,

	AuthResponse     = 20
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

