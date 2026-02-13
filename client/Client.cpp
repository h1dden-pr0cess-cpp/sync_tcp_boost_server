#include <boost/asio.hpp>
#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

enum class PacketType : uint8_t
{
    Ping             = 1,
	UploadStart      = 2,
    UploadChunk      = 3,
	UoloadEnd        = 4,
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

// Функция для сериализации строки в формат [uint16_t len][data]
std::vector<uint8_t> write_string(const std::string& s) {
    std::vector<uint8_t> buf;
    uint16_t len = static_cast<uint16_t>(s.size());
    buf.resize(sizeof(len) + s.size());
    std::memcpy(buf.data(), &len, sizeof(len));
    std::memcpy(buf.data() + sizeof(len), s.data(), s.size());
    return buf;
}

// Отправка пакета: [PacketType][uint32_t size][body]
void send_packet(boost::asio::ip::tcp::socket& sock, PacketType type, const std::vector<uint8_t>& body) {
    uint32_t size = static_cast<uint32_t>(body.size());
    std::vector<uint8_t> packet(1 + 4 + body.size()); // 1 байт type + 4 байта size + body

    packet[0] = static_cast<uint8_t>(type);
    std::memcpy(packet.data() + 1, &size, sizeof(size));
    if (!body.empty()) {
        std::memcpy(packet.data() + 5, body.data(), body.size());
    }

    boost::asio::write(sock, boost::asio::buffer(packet));
}

int main() {
    try {
        boost::asio::io_context io;

        boost::asio::ip::tcp::socket socket(io);
        // ✅ Используем make_address
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address("127.0.0.1"), 12345);
        socket.connect(endpoint);

        // 🔹 Отправляем Ping
        send_packet(socket, PacketType::Ping, {});

        // 🔹 Логин с паролем
        std::vector<uint8_t> body;
        auto username = write_string("test_user");
        auto password = write_string("test_password_hash");

        body.insert(body.end(), username.begin(), username.end());
        body.insert(body.end(), password.begin(), password.end());

        send_packet(socket, PacketType::LoginWithPassword, body);

        std::cout << "Packets sent!\n";
    } catch (std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}

