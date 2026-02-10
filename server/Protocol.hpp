#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <string>
#include <cstdint>

enum class PacketType : uint8_t
{
    Ping             = 1,
    UploadChunk      = 2,
    Download         = 3,
    ListFiles        = 4,

    RegisterUser     = 10, 
    LoginWithPassword= 11, 
    LoginWithToken   = 12, 
    Logout           = 13  
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
	std::string token;  };

#endif // PROTOCOL_HPP

