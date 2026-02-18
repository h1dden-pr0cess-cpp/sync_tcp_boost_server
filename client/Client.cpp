#include <boost/asio.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

using boost::asio::ip::tcp;

/* =========================
   Вспомогательные функции
   ========================= */

void append_string(std::vector<uint8_t>& body, const std::string& s)
{
    uint16_t len = static_cast<uint16_t>(s.size());
    body.push_back(len & 0xFF);
    body.push_back((len >> 8) & 0xFF);
    body.insert(body.end(), s.begin(), s.end());
}

std::string read_string(const std::vector<uint8_t>& body, size_t& offset)
{
    uint16_t len = body[offset] | (body[offset + 1] << 8);
    offset += 2;

    std::string s(body.begin() + offset, body.begin() + offset + len);
    offset += len;

    return s;
}

void send_packet(tcp::socket& socket,
                 uint8_t type,
                 const std::vector<uint8_t>& body)
{
    uint32_t size = static_cast<uint32_t>(body.size());

    std::vector<uint8_t> buffer;
    buffer.resize(1 + 4 + size);

    buffer[0] = type;
    std::memcpy(buffer.data() + 1, &size, 4);
    if (size > 0)
        std::memcpy(buffer.data() + 5, body.data(), size);

    boost::asio::write(socket, boost::asio::buffer(buffer));
}

bool read_packet(tcp::socket& socket,
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
    if (size > 0)
        boost::asio::read(socket, boost::asio::buffer(body), ec);

    return !ec;
}

/* =========================
   main
   ========================= */

int main()
{
    const std::string host = "127.0.0.1";
    const std::string port = "12345";

    const uint8_t LOGIN_WITH_PASSWORD = 11; // поставь свои значения
    const uint8_t LOGIN_WITH_TOKEN    = 12;
    const uint8_t AUTH_RESPONSE       = 20;

    std::string token;

    /* ======== 1. Логин по паролю ======== */

    {
        boost::asio::io_context io;
        tcp::resolver resolver(io);
        tcp::socket socket(io);

        auto endpoints = resolver.resolve(host, port);
        boost::asio::connect(socket, endpoints);

        std::vector<uint8_t> body;
        append_string(body, "testuser");
        append_string(body, "123");

        send_packet(socket, LOGIN_WITH_PASSWORD, body);

        uint8_t type;
        std::vector<uint8_t> resp;

        if (!read_packet(socket, type, resp)) {
            std::cout << "Login response read failed\n";
            return 1;
        }

        if (type != AUTH_RESPONSE) {
            std::cout << "Unexpected packet type\n";
            return 1;
        }

        size_t offset = 0;
        token = read_string(resp, offset);

        std::cout << "Received token: " << token << "\n";
    }

    /* ======== 2. Новый коннект + логин по токену ======== */

    {
        boost::asio::io_context io;
        tcp::resolver resolver(io);
        tcp::socket socket(io);

        auto endpoints = resolver.resolve(host, port);
        boost::asio::connect(socket, endpoints);

        std::vector<uint8_t> body;
        append_string(body, token);

        send_packet(socket, LOGIN_WITH_TOKEN, body);

        uint8_t type;
        std::vector<uint8_t> resp;

        if (!read_packet(socket, type, resp)) {
            std::cout << "Token login failed\n";
            return 1;
        }

        if (type == AUTH_RESPONSE)
            std::cout << "Token login SUCCESS\n";
        else
            std::cout << "Token login FAILED\n";
    }

    return 0;
}
