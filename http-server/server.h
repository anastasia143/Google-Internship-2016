#pragma once

#include <iostream>
#include <string>

#include <boost/system/error_code.hpp>
#include <boost/date_time/date.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/asio.hpp>
#include <boost/asio/async_result.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/basic_resolver_query.hpp>

#include "requestHandler.h"
#include "session.h"

/// Class for the HTTP-server
class Server : private boost::noncopyable
{
public:
	explicit Server(const boost::asio::ip::address &address
			, short unsigned int &port
			, const std::string &docRoot
			, std::size_t threadCount);

	/// Run the server's io_service loop.
	void run();

	/// Stop the server.
	void stop();
private:
	 /// Acceptor used to listen for incoming connections.
	std::size_t mThreadCount;

	/// The io_service used to perform asynchronous operations.
	boost::asio::io_service mIoService;

	 /// Acceptor used to listen for incoming connections.
	boost::asio::ip::tcp::acceptor mAcceptor;

	/// Handle completion of an asynchronous accept operation.
	void handleAccept(const boost::system::error_code& err);

	/// The next connection to be accepted.
	session_ptr mNewConnection;

	/// The handler for all incoming requests.
	RequestHandler mRequestHandler;
};
