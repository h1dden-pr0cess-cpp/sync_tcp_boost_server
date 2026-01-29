#ifndef SESSION_HPP
#define SESSION_HPP

#include <boost/asio.hpp>
#include <memory>
#include <array>

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(
        int id,
        boost::asio::ip::tcp::socket socket
    );

    void start();

    int id() const;

private:
    void do_read();

    int id_;
    boost::asio::ip::tcp::socket socket_;
};

#endif // SESSION_HPP

