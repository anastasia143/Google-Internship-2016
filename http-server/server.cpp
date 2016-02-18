#include "server.h"

Server::Server(const boost::asio::ip::address &address
		, short unsigned int &port
		, const std::string &docRoot
		, std::size_t threadCount)
	: mThreadCount(threadCount)
	, mAcceptor(mIoService)
{
	// socket opening
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
	mAcceptor.open(endpoint.protocol());
	mAcceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	mAcceptor.bind(endpoint);
	mAcceptor.listen();
}

