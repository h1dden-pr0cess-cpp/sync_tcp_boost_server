#include <boost/asio.hpp>
#include "Server.hpp"

int main()
{
	boost::asio::io_context io;
	Server server(io, 12345);
	io.run();
	return 0;
}

