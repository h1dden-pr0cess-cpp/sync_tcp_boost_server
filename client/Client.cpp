#include <boost/asio.hpp>
#include <iostream>
#include <string>

void send_data(boost::asio::ip::tcp::socket& socket)
{
    std::string message;
    std::getline(std::cin, message);

    message += '\n';

    boost::asio::write(
        socket,
        boost::asio::buffer(message)
    );

    std::cout << "Data sent\n";
}

int main()
{
    try
    {
        boost::asio::io_context io;

        boost::asio::ip::tcp::socket socket(io);

        socket.connect(
            boost::asio::ip::tcp::endpoint(
                boost::asio::ip::make_address("127.0.0.1"),
                12345
            )
        );

        while (true)
        {
            send_data(socket);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
																	                    
