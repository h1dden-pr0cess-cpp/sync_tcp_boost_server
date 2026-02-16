#include "Session.hpp"
#include "Server.hpp"

Session::Session(int id, boost::asio::ip::tcp::socket socket, Server& server)
    : id_(id),
      socket_(std::move(socket)),
	  server_(server)
{}

int Session::id() const {
    return id_;
}

void Session::start() {
    read_header();
}


static std::string read_string(const std::vector<uint8_t>& buf, size_t& offset)
{
    if (offset + sizeof(uint16_t) > buf.size())
        throw std::runtime_error("Bad packet (string length)");

    uint16_t len;
    std::memcpy(&len, &buf[offset], sizeof(len));
    offset += sizeof(len);

    if (offset + len > buf.size())
        throw std::runtime_error("Bad packet (string data)");

    std::string s(reinterpret_cast<const char*>(&buf[offset]), len);
    offset += len;

    return s;
}

void Session::read_header() {
    auto self = shared_from_this();

    boost::asio::async_read(
        socket_,
        boost::asio::buffer(&packet_type_, sizeof(packet_type_)),
        [self](boost::system::error_code ec, std::size_t) {
            if (ec) {
				self->socket_.close();
				return;
			}

            boost::asio::async_read(
                self->socket_,
                boost::asio::buffer(&self->packet_size_, sizeof(self->packet_size_)),
                [self](boost::system::error_code ec, std::size_t) {
                    if (ec) {
						self->socket_.close();
						return;
					}

                    self->body_.resize(self->packet_size_);
                    self->read_body();
                }
            );
        }
    );
}


void Session::read_body() {
	if(packet_size_ > MAX_PACKET_SIZE)
	{
		std::cout << "Packet too big!\n";
		socket_.close();
		return;
	}

    auto self = shared_from_this();

    if (packet_size_ == 0) {
        handle_packet();
        read_header();
        return;
    }

	body_.resize(packet_size_);

    boost::asio::async_read(
        socket_,
		boost::asio::buffer(body_),
		[self](boost::system::error_code ec, std::size_t) {
            if (ec) {
			self->socket_.close();
			return;
		}

            self->handle_packet();
            self->read_header();
        }
    );
}


void Session::handle_packet() {
    PacketType type = static_cast<PacketType>(packet_type_);

    // 🔒 если не авторизован — разрешаем только логин/регистрацию
    if (!authorized_ &&
        type != PacketType::RegisterUser &&
        type != PacketType::LoginWithPassword &&
        type != PacketType::LoginWithToken)
    {
        std::cout << "Client " << id_ << " not authorized\n";
        return;
    }

    switch(static_cast<PacketType>(packet_type_)) {
		case PacketType::RegisterUser: {
			handle_register(body_);
			break;
		}
		case PacketType::LoginWithPassword: {
			handle_login_password(body_);
			break;
		}
		case PacketType::LoginWithToken: {
			handle_login_token(body_);
			break;
		}
		case PacketType::Logout: {
			authorized_ = false;
			username_.clear();
			std::cout << "Client " << id_ << " logged out\n";
			break;
		}
		case PacketType::UploadStart: {
			handle_upload_start(body_);
			break;
		}
		case PacketType::UploadChunk: {
			handle_upload_chunk(body_);
			break;
		}
		case PacketType::UploadEnd: {
			handle_upload_end(body_);
			break;
		}
		case PacketType::Download: {
			std::cout << "Download request from user " << username_ << "\n";
			break;
		}
		case PacketType::ListFiles: {
			std::cout << "List files request from user " << username_ << "\n";
			break;
		}
		default: {
			std::cout << "Unknown packet from client " << id_ << "\n";
			break;
		}
	}
}

void Session::handle_register(const std::vector<uint8_t>& body) {
    try {
        size_t offset = 0;
        std::string username = read_string(body, offset);
        std::string password_hash = read_string(body, offset);

        if (server_.user_exists(username)) {
            std::cout << "User already exists: " << username << "\n";
            // можно отправить пакет-ответ с ошибкой
            return;
        }

        User user{username, password_hash};
        server_.add_user(user);

        std::cout << "Registered user: " << username << "\n";

        Packet response;
        response.type = PacketType::AuthResponse; 
		send_packet(response);

    } catch (const std::exception& e) {
        std::cout << "Bad Register packet: " << e.what() << "\n";
    }
}

void Session::handle_login_password(const std::vector<uint8_t>& body)
{
    try {
        size_t offset = 0;
        std::string username = read_string(body, offset);
        std::string password_hash = read_string(body, offset);

        if (!server_.check_user(username, password_hash)) {
            std::cout << "Login failed: " << username << "\n";
            // можно отправить пакет-ответ с ошибкой
            return;
        }

        authorized_ = true;
        username_ = username;

        std::cout << "Login successful: " << username << "\n";

        Packet response;
        response.type = PacketType::AuthResponse; 
		send_packet(response);

    } catch (const std::exception& e) {
        std::cout << "Bad Login packet: " << e.what() << "\n";
    }}


void Session::handle_login_token(const std::vector<uint8_t>& body)
{
    try {
        size_t offset = 0;

        std::string token = read_string(body, offset);

        std::cout << "LoginWithToken: token=" << token << "\n";

        // TODO: проверить токен
        authorized_ = true;
        username_ = "user_from_token";
    }
    catch (const std::exception& e) {
        std::cout << "Bad LoginWithToken packet: " << e.what() << "\n";
    }
}



void Session::handle_upload_start(const std::vector<uint8_t>& body)
{
	size_t offset = 0;
	current_upload_.filename = read_string(body, offset);
	current_upload_.data.clear();
	current_upload_.active = true;

	std::cout << "Upload started: " << current_upload_.filename << "\n";
}

void Session::handle_upload_chunk(const std::vector<uint8_t>& body)
{
	if(!current_upload_.active){
		std::cout << "No active upload\n";
		return;
	}
	if(body.size() > MAX_PACKET_SIZE) {
		std::cout << "Chunk too big\n";
		return;
	}
	current_upload_.data.insert(current_upload_.data.end(), body.begin(), body.end());
	std::cout << "Received chunk, size = " << body.size() 
		<< ", total = " << current_upload_.data.size() << "\n";
}

void Session::handle_upload_end(const std::vector<uint8_t>& body)
{
	if(!current_upload_.active) return;

	std::cout << "Upload finished: " << current_upload_.filename
		<< ", size = " << current_upload_.data.size() << "\n";

	save_file();

	current_upload_.active = false;
	current_upload_.data.clear();
}
void Session::save_file()
{
	std::ofstream out(current_upload_.filename, std::ios::binary);
    out.write(reinterpret_cast<const char*>(current_upload_.data.data()), current_upload_.data.size());
}



void Session::send_packet(const Packet& packet)
{
	uint32_t size = static_cast<uint32_t>(packet.body.size());

    auto buffer = std::make_shared<std::vector<uint8_t>>(1 + 4 + size);

    (*buffer)[0] = static_cast<uint8_t>(packet.type);
    std::memcpy(buffer->data() + 1, &size, sizeof(size));

    if (size > 0)
        std::memcpy(buffer->data() + 5, packet.body.data(), size);

    auto self = shared_from_this();

    boost::asio::async_write(
        socket_,
        boost::asio::buffer(*buffer),
        [self, buffer](boost::system::error_code ec, std::size_t)
        {
            if (ec)
                self->socket_.close();
        }
    );
}

