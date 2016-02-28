#include "server.h"

Server::Server(const boost::asio::ip::address &address
		, short unsigned int &port
		, const std::string &docRoot
		, std::size_t threadCount)
	: mThreadCount(threadCount)
	, mAcceptor(mIoService)
	, mRequestHandler(docRoot)
	, mNewConnection(new Session(mIoService, mRequestHandler))
{
	// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
	boost::asio::ip::tcp::resolver resolver(mIoService);
	boost::asio::ip::tcp::resolver::query query(address.to_string(), std::to_string(port));
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
	mAcceptor.open(endpoint.protocol());
	mAcceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	mAcceptor.bind(endpoint);
	mAcceptor.listen();
	mAcceptor.async_accept(mNewConnection->socket()
			, boost::bind(&Server::handleAccept, this, boost::asio::placeholders::error));
}

void Server::run()
{
	// Create a pool of threads to run all of the io_services.
	std::vector<boost::shared_ptr<boost::thread> > threads;
	for (std::size_t i = 0; i < mThreadCount; ++i) {
		boost::shared_ptr<boost::thread> thread(new boost::thread(
			boost::bind(&boost::asio::io_service::run, &mIoService)));
		threads.push_back(thread);
	}

	// Wait for all threads in the pool to exit.
	for (std::size_t i = 0; i < threads.size(); ++i)
		threads[i]->join();
}

void Server::stop()
{
	mIoService.stop();
}

void Server::handleAccept(boost::system::error_code const &err)
{
	if (!err) {
		mNewConnection->start();
		mNewConnection.reset(new Session(mIoService, mRequestHandler));
		mAcceptor.async_accept(mNewConnection->socket()
				, boost::bind(&Server::handleAccept, this, boost::asio::placeholders::error));
  }
}

