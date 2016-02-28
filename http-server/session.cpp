#include "session.h"

Session::Session(boost::asio::io_service& ioService, RequestHandler &handler)
	: mStrand(ioService)
	, mSocket(ioService)
	, mRequestHandler(handler)
{
}

boost::asio::ip::tcp::socket& Session::socket()
{
	return mSocket;
}

void Session::start()
{
	mSocket.async_read_some(boost::asio::buffer(mBuffer), mStrand.wrap(
		boost::bind(&Session::handleRead, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred)));
}

void Session::handleRead(boost::system::error_code const &err,
	std::size_t bytesTransferred)
{
  if (!err)
  {
	boost::tribool result;
	boost::tie(result, boost::tuples::ignore) = mRequestParser.parse(
		mRequest, mBuffer.data(), mBuffer.data() + bytesTransferred);

	if (result)
	{
	  mRequestHandler.handleRequest(mRequest, mReply);
	  boost::asio::async_write(mSocket, mReply.toBuffers(),
		  mStrand.wrap(
			boost::bind(&Session::handleWrite, shared_from_this(),
			  boost::asio::placeholders::error)));
	}
	else if (!result)
	{
	  mReply = Reply::stockReply(Reply::BadRequest);
	  boost::asio::async_write(mSocket, mReply.toBuffers(),
		  mStrand.wrap(
			boost::bind(&Session::handleWrite, shared_from_this(),
			  boost::asio::placeholders::error)));
	}
	else
	{
	  mSocket.async_read_some(boost::asio::buffer(mBuffer),
		  mStrand.wrap(
			boost::bind(&Session::handleRead, shared_from_this(),
			  boost::asio::placeholders::error,
			  boost::asio::placeholders::bytes_transferred)));
	}
  }

  // If an error occurs then no new asynchronous operations are started. This
  // means that all shared_ptr references to the Session object will
  // disappear and the object will be destroyed automatically after this
  // handler returns. The Session class's destructor closes the socket.
}

void Session::handleWrite(boost::system::error_code const &err)
{
  if (!err)
  {
	// Initiate Session closure.
	boost::system::error_code ignoredEc;
	mSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignoredEc);
  }

  // No new asynchronous operations are started. This means that all shared_ptr
  // references to the Session object will disappear and the object will be
  // destroyed automatically after this handler returns. The Session class's
  // destructor closes the socket.
}
