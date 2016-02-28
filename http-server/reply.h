#pragma once

#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

#include "header.h"

class Reply
{
public:
	Reply() {}

	enum Status {
		Ok = 200,
		Created = 201,
		Accepted = 202,
		NoContent = 204,
		MultipleChoices = 300,
		MovedPermanently = 301,
		MovedTemporarily = 302,
		NotModified = 304,
		BadRequest = 400,
		Unauthorized = 401,
		Forbidden = 403,
		NotFound = 404,
		InternalServerError = 500,
		NotImplemented = 501,
		BadGateway = 502,
		ServiceUnavailable = 503
	} status;

  /// The headers to be included in the reply.
  std::vector<Header> headers;

  /// The content to be sent in the reply.
  std::string content;

  /// Convert the reply into a vector of buffers. The buffers do not own the
  /// underlying memory blocks, therefore the reply object must remain valid and
  /// not be changed until the write operation has completed.
  std::vector<boost::asio::const_buffer> toBuffers();

  /// Get a stock reply.
  static Reply stockReply(Status status);
};
