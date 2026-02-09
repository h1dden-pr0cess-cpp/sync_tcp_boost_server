#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>

class Server
{
public:
    Server(boost::asio::io_context& io, unsigned short port);

private:
    void do_accept();

    boost::asio::ip::tcp::acceptor acceptor_;
    int next_id_;
};

#endif // SERVER_HPP

