#include <boost/asio.hpp>
#include <iostream>
#include <vector>
#include <cstdint>

using boost::asio::ip::tcp;

/* ===== packet_type ===== */
enum class PacketType : uint8_t {
    Ping = 1,
    Login = 2,
    SaveUpload = 3
};

/* ===== запись uint32_t в buffer (network byte order) ===== */
void write_u32(std::vector<uint8_t>& buf, uint32_t value) {
    buf.push_back((value >> 24) & 0xFF);
    buf.push_back((value >> 16) & 0xFF);
    buf.push_back((value >> 8) & 0xFF);
    buf.push_back(value & 0xFF);
}

int main() {
    try {
        boost::asio::io_context io;
        tcp::socket socket(io);

        socket.connect({
            boost::asio::ip::make_address("127.0.0.1"),
            12345
        });

        /* ===== формируем пакет =====
           [1 byte ] packet_type
           [4 bytes] body_size
           [N bytes] body (пусто)
        */
        std::vector<uint8_t> packet;

        packet.push_back(static_cast<uint8_t>(PacketType::Ping));
        write_u32(packet, 0); // body_size = 0

        boost::asio::write(socket, boost::asio::buffer(packet));

        std::cout << "packet sent\n";
    }
    catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
    }
}

