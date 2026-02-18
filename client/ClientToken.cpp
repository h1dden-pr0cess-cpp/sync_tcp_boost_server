#include <boost/asio.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

// функция для сериализации строки в body
void append_string(std::vector<uint8_t>& body, const std::string& s) {
    uint16_t len = static_cast<uint16_t>(s.size());
    body.push_back(len & 0xFF);
    body.push_back((len >> 8) & 0xFF);
    body.insert(body.end(), s.begin(), s.end());
}

// отправка пакета
void send_packet(boost::asio::ip::tcp::socket& socket,
                 uint8_t type,
                 const std::vector<uint8_t>& body) 
{
    uint32_t size = static_cast<uint32_t>(body.size());
    std::vector<uint8_t> buffer(1 + 4 + body.size());
    buffer[0] = type;
    std::memcpy(buffer.data() + 1, &size, 4);
    if (!body.empty()) {
        std::memcpy(buffer.data() + 5, body.data(), body.size());
    }
    boost::asio::write(socket, boost::asio::buffer(buffer));
}

// чтение пакета (тип + размер + тело)
bool read_packet(boost::asio::ip::tcp::socket& socket,
                 uint8_t& type,
                 std::vector<uint8_t>& body)
{
    boost::system::error_code ec;
    boost::asio::read(socket, boost::asio::buffer(&type, 1), ec);
    if (ec) return false;

    uint32_t size = 0;
    boost::asio::read(socket, boost::asio::buffer(&size, 4), ec);
    if (ec) return false;

    body.resize(size);
    if (size > 0) {
        boost::asio::read(socket, boost::asio::buffer(body), ec);
        if (ec) return false;
    }
    return true;
}

// функция для чтения строки из body
std::string read_string(const std::vector<uint8_t>& body, size_t& offset) {
    if (offset + 2 > body.size()) throw std::runtime_error("Bad packet");
    uint16_t len = body[offset] | (body[offset+1] << 8);
    offset += 2;
    if (offset + len > body.size()) throw std::runtime_error("Bad packet");
    std::string s(body.begin() + offset, body.begin() + offset + len);
    offset += len;
    return s;
}

int main() {
    try {
        boost::asio::io_context io;
        boost::asio::ip::tcp::socket socket(io);
        boost::asio::ip::tcp::resolver resolver(io);
        auto endpoints = resolver.resolve("127.0.0.1", "12345");
        boost::asio::connect(socket, endpoints);

        // ===== логин по имени и паролю =====
        std::string username = "testuser";
        std::string password_hash = "123";

        std::vector<uint8_t> body;
        append_string(body, username);
        append_string(body, password_hash);

        uint8_t LOGIN_WITH_PASSWORD = 2; // по твоему enum PacketType
        send_packet(socket, LOGIN_WITH_PASSWORD, body);

        // читаем ответ (предположим, что сервер вернул токен)
        uint8_t resp_type;
        std::vector<uint8_t> resp_body;
        if (!read_packet(socket, resp_type, resp_body)) {
            std::cout << "Failed to read login response\n";
            return 1;
        }

        size_t offset = 0;
        std::string token = read_string(resp_body, offset);
        std::cout << "Received token: " << token << "\n";

        // ===== повторный вход через токен =====
        std::vector<uint8_t> token_body;
        append_string(token_body, token);

        uint8_t LOGIN_WITH_TOKEN = 3; // по твоему enum PacketType
        send_packet(socket, LOGIN_WITH_TOKEN, token_body);

        std::cout << "Token sent to server\n";

    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}
