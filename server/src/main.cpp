#include <boost/asio.hpp>
#include "Server.hpp"
#include "Database.hpp"

int main()
{
	boost::asio::io_context io;

	Database db("users.db");
	Server server(io, 12345, db);
	io.run();

	return 0;
}

