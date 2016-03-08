#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>

#include "server.h"
#include "requestHandler.h"

boost::function0<void> console_ctrl_function;

BOOL WINAPI console_ctrl_handler(DWORD ctrl_type)
{
	switch (ctrl_type) {
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		console_ctrl_function();
		return TRUE;
	default:
		return FALSE;
	}
}

int main()
{
	try {
		// Initialise server.
		int threadsNum = 4;
		boost::asio::ip::address addr = boost::asio::ip::address_v4::from_string("127.0.0.1");
		short unsigned int port = 80;
		const std::string docRoot = "C:/PROGRAMMING/Google-Internship-2016/root";
		Server server(addr, port, docRoot, threadsNum);


		// Set console control handler to allow server to be stopped.
		console_ctrl_function = boost::bind(&Server::stop, &server);
		SetConsoleCtrlHandler(console_ctrl_handler, TRUE);

		// Run the server until stopped.
		server.run();
	}
	catch (std::exception& err) {
		std::cerr << "exception: " << err.what() << "\n";
	}
	return 0;
}
