#pragma once

#include <vector>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>

#include "requestHandler.h"
#include "requestParser.h"

/// @class Session implements single connection with the client
/* std::enable_shared_from_this позволяет объекту t, который управляется умным указателем pt
с типом std::shared_ptr, безопасно создать дополнительные экземпляры pt1, pt2, ...
с типом std::shared_ptr, такие, что все они будут раздельно владеть объектом t, наравне с pt.*/
class Session : public boost::enable_shared_from_this<Session>, private boost::noncopyable
{
public:
	/// Construct a connection with the given io_service.
	explicit Session(boost::asio::io_service &ioService, RequestHandler &handler);

	/// Get the socket associated with the connection.
	boost::asio::ip::tcp::socket &socket();

	/// Start the first asynchronous operation for the connection.
	void start();

private:
	  /// Handle completion of a read operation.
	  void handleRead(boost::system::error_code const &err,
		  std::size_t bytes_transferred);

	  /// Handle completion of a write operation.
	  void handleWrite(boost::system::error_code const &err);

	  /// Strand to ensure the connection's handlers are not called concurrently.
	  boost::asio::io_service::strand mStrand;

	  /// Socket for the connection.
	  boost::asio::ip::tcp::socket mSocket;

	  /// The handler used to process the incoming request.
	  RequestHandler &mRequestHandler;

	  /// Buffer for incoming data.
	  boost::array<char, 8192> mBuffer;

	  /// The incoming request.
	  Request mRequest;

	  /// The parser for the incoming request.
	  RequestParser mRequestParser;

	  /// The reply to be sent back to the client.
	  Reply mReply;

private:

};

typedef boost::shared_ptr<Session> session_ptr;
