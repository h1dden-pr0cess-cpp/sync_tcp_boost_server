#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <cstdint>

enum class PacketType : uint8_t
{
    Ping        = 1,
    UploadChunk = 2,
    Download    = 3,
    ListFiles   = 4
};

#endif // PROTOCOL_HPP

