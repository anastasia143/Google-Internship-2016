#pragma once

#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>

#include "request.h"

class RequestParser
{
public:
	/// Construct ready to parse the request method.
	RequestParser();

  /// Reset to initial parser state.
  void reset();

  /// Parse some data. The tribool return value is true when a complete request
  /// has been parsed, false if the data is invalid, indeterminate when more
  /// data is required. The InputIterator return value indicates how much of the
  /// input has been consumed.
  template <typename InputIterator>
  boost::tuple<boost::tribool, InputIterator> parse(Request &request
		, InputIterator begin, InputIterator end) {
	while (begin != end) {
		boost::tribool result = consume(request, *begin++);
		if (result || !result)
			return boost::make_tuple(result, begin);
	}
	boost::tribool result = boost::indeterminate;
	return boost::make_tuple(result, begin);
}

private:
	/// Handle the next character of input.
	boost::tribool consume(Request &request, char input);

	/// Check if a byte is an HTTP character.
	bool isChar(int c);

	/// Check if a byte is an HTTP control character.
	bool isCtl(int c);

	/// Check if a byte is defined as an HTTP tspecial character.
	bool isTspecial(int c);

	/// Check if a byte is a digit.
	bool isDigit(int c);

	/// The current state of the parser.
	enum State {
		methodStart,
		method,
		uriStart,
		uri,
		httpVersion_h,
		httpVersion_t_1,
		httpVersion_t_2,
		httpVersion_p,
		httpVersionSlash,
		httpVersionMajorStart,
		httpVersionMajor,
		httpVersionMinorStart,
		httpVersionMinor,
		expectingNewline_1,
		headerLineStart,
		headerLws,
		headerName,
		spaceBeforeHeaderValue,
		headerValue,
		expectingNewline_2,
		expectingNewline_3
  } mState;
};
