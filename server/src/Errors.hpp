#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <string>
#include <cstdint>
#include <vector>
#include "Protocol.hpp"

enum class ErrorCode : uint8_t {
    Unknown = 0,

    InvalidPacket    = 1,
    InvalidToken     = 2,
    UserAlreadyExist = 3,
    InvalidLogin     = 4,
    GameNotFound     = 5,
    CannotAddGame    = 6,
    SaveNotFound     = 7,
    CannotAddSave    = 8,

    DatabaseError    = 10,

    UnknownPacket    = 20
};

class Errors
{
    public:
    
    Packet send_error(ErrorCode code);

    private:
    void write_error(std::vector<uint8_t>& error,
                     const ErrorCode error_type);

    void convert_error(std::vector<uint8_t>& error, 
                       const std::string error_str);
};

#endif
