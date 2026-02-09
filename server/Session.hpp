#ifndef SESSION_HPP
#define SESSION_HPP

#include <boost/asio.hpp>
#include <memory>
#include <array>

class Session : public std::enable_shared_from_this<Session>
{
public:
    void start();

private:
    void read_size();
    void read_body();

    uint32_t current_size_;
    std::vector<char> body_;

    boost::asio::ip::tcp::socket socket_;
};


#endif // SESSION_HPP

