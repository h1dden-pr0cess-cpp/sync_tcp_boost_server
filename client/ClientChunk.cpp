#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

enum class PacketType : uint8_t
{
    UploadStart       = 2,
    UploadChunk       = 3,
    UploadEnd         = 4,

    RegisterUser      = 10,
    LoginWithPassword = 11,
    AuthResponse      = 20
};

void send_packet(boost::asio::ip::tcp::socket& socket,
                 PacketType type,
                 const std::vector<uint8_t>& body)
{
    uint32_t size = static_cast<uint32_t>(body.size());

    std::vector<uint8_t> buffer(1 + 4 + size);

    buffer[0] = static_cast<uint8_t>(type);
    std::memcpy(buffer.data() + 1, &size, sizeof(size));

    if (size > 0)
        std::memcpy(buffer.data() + 5, body.data(), size);

    boost::asio::write(socket, boost::asio::buffer(buffer));
}

void append_string(std::vector<uint8_t>& body, const std::string& s)
{
    uint16_t len = static_cast<uint16_t>(s.size());
    body.push_back(len & 0xFF);
    body.push_back((len >> 8) & 0xFF);
    body.insert(body.end(), s.begin(), s.end());
}

bool read_auth_response(boost::asio::ip::tcp::socket& socket)
{
    uint8_t type;
    uint32_t size;

    boost::asio::read(socket, boost::asio::buffer(&type, 1));
    boost::asio::read(socket, boost::asio::buffer(&size, 4));

    std::vector<uint8_t> body(size);
    if (size > 0)
        boost::asio::read(socket, boost::asio::buffer(body));

    if (static_cast<PacketType>(type) != PacketType::AuthResponse)
    {
        std::cout << "Unexpected packet type\n";
        return false;
    }

    std::cout << "AuthResponse received\n";
    return true;
}

int main()
{
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket(io_context);

    socket.connect(
        boost::asio::ip::tcp::endpoint(
            boost::asio::ip::make_address("127.0.0.1"),
            12345
        )
    );

    std::string username = "testuser";
    std::string password = "123456";

    // 🔹 1. Register
    {
        std::vector<uint8_t> body;
        append_string(body, username);
        append_string(body, password);

        send_packet(socket, PacketType::RegisterUser, body);

        if (!read_auth_response(socket))
            return 1;
    }

    // 🔹 2. Login
    {
        std::vector<uint8_t> body;
        append_string(body, username);
        append_string(body, password);

        send_packet(socket, PacketType::LoginWithPassword, body);

        if (!read_auth_response(socket))
            return 1;
    }

    std::cout << "Authorized successfully\n";

    // 🔹 3. UploadStart
    std::string filename = "test.txt";
    {
        std::vector<uint8_t> body;
        append_string(body, filename);
        send_packet(socket, PacketType::UploadStart, body);
    }

    // 🔹 4. UploadChunk
    std::ifstream file(filename, std::ios::binary);
    const std::size_t CHUNK_SIZE = 64 * 1024;
    std::vector<uint8_t> chunk_buffer(CHUNK_SIZE);

    while (file)
    {
        file.read(reinterpret_cast<char*>(chunk_buffer.data()), CHUNK_SIZE);
        std::streamsize bytes_read = file.gcount();

        if (bytes_read > 0)
        {
            std::vector<uint8_t> body(
                chunk_buffer.begin(),
                chunk_buffer.begin() + bytes_read
            );

            send_packet(socket, PacketType::UploadChunk, body);
        }
    }

    // 🔹 5. UploadEnd
    {
        std::vector<uint8_t> body;
        send_packet(socket, PacketType::UploadEnd, body);
    }

    std::cout << "Upload finished\n";

    return 0;
}
