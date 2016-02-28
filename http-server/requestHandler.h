#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>

#include "reply.h"
#include "request.h"
#include "mimeTypes.h"

/// @class  RequestHandler represents handler for all incoming requests.
class RequestHandler : private boost::noncopyable
{
public:
  /// Construct with a directory containing files to be served.
  explicit RequestHandler(const std::string &docRoot);

  /// Handle a request and produce a reply.
  void handleRequest(Request const &request, Reply &reply);

//private:
  /// The directory containing the files to be served.
  std::string mDocRoot;

  /// Perform URL-decoding on a string. Returns false if the encoding was invalid.
  bool urlDecode(std::string const &in, std::string &out);
};
