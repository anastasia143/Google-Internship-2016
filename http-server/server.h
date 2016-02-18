#pragma once

#include <iostream>
#include <string>

#include <boost/system/error_code.hpp>
#include <boost/date_time/date.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind/bind.hpp>
#include <boost/lexical_cast/lexical_cast_old.hpp>

#include <boost/asio/async_result.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>


/// Class for the HTTP-server
class Server
{
public:
	explicit Server(const boost::asio::ip::address &address
			, short unsigned int &port
			, const std::string &docRoot
			, std::size_t threadCount);
private:
	std::size_t mThreadCount;
	boost::asio::io_service mIoService;
	boost::asio::ip::tcp::acceptor mAcceptor;
};
