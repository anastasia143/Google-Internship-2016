#include "requestParser.h"

RequestParser::RequestParser()
	: mState(methodStart)
{
}

void RequestParser::reset()
{
	mState = methodStart;
}

boost::tribool RequestParser::consume(Request &request, char input)
{
	switch (mState)
	{
	case methodStart:
		if (!isChar(input) || isCtl(input) || isTspecial(input)) {
			return false;
		}
		else {
			mState = method;
			request.mMethod.push_back(input);
			return boost::indeterminate;
		}
	case method:
		if (input == ' ') {
			mState = uri;
			return boost::indeterminate;
		}
		else if (!isChar(input) || isCtl(input) || isTspecial(input)) {
			return false;
		}
		else {
			request.mMethod.push_back(input);
			return boost::indeterminate;
		}
	case uriStart:
		if (isCtl(input)) {
			return false;
		}
		else {
			mState = uri;
			request.mUri.push_back(input);
			return boost::indeterminate;
		}
	case uri:
		if (input == ' ') {
			mState = httpVersion_h;
			return boost::indeterminate;
		}
		else if (isCtl(input)) {
			return false;
		}
		else {
			request.mUri.push_back(input);
			return boost::indeterminate;
		}
	case httpVersion_h:
		if (input == 'H') {
			mState = httpVersion_t_1;
			return boost::indeterminate;
		}
		else {
			return false;
		}
	case httpVersion_t_1:
		if (input == 'T') {
			mState = httpVersion_t_2;
			return boost::indeterminate;
		}
		else {
			return false;
		}
	case httpVersion_t_2:
		if (input == 'T') {
			mState = httpVersion_p;
			return boost::indeterminate;
		}
		else {
			return false;
		}
	case httpVersion_p:
		if (input == 'P') {
			mState = httpVersionSlash;
			return boost::indeterminate;
		}
		else {
			return false;
		}
	case httpVersionSlash:
		if (input == '/') {
			request.httpVersionMajor = 0;
			request.httpVersionMinor = 0;
			mState = httpVersionMajorStart;
			return boost::indeterminate;
		}
		else {
			return false;
		}
	case httpVersionMajorStart:
		if (isDigit(input)) {
			request.httpVersionMajor = request.httpVersionMajor * 10 + input - '0';
			mState = httpVersionMajor;
			return boost::indeterminate;
		}
		else {
			return false;
		}
	case httpVersionMajor:
		if (input == '.') {
			mState = httpVersionMinorStart;
			return boost::indeterminate;
		}
		else if (isDigit(input)) {
			request.httpVersionMajor = request.httpVersionMajor * 10 + input - '0';
			return boost::indeterminate;
		}
		else {
			return false;
		}
	case httpVersionMinorStart:
		if (isDigit(input)) {
			request.httpVersionMinor = request.httpVersionMinor * 10 + input - '0';
			mState = httpVersionMinor;
			return boost::indeterminate;
		}
		else {
			return false;
		}
	case httpVersionMinor:
		if (input == '\r') {
			mState = expectingNewline_1;
			return boost::indeterminate;
		}
		else if (isDigit(input)) {
			request.httpVersionMinor = request.httpVersionMinor * 10 + input - '0';
			return boost::indeterminate;
		}
		else {
			return false;
		}
	case expectingNewline_1:
		if (input == '\n') {
			mState = headerLineStart;
			return boost::indeterminate;
		}
		else {
			return false;
		}
	case headerLineStart:
		if (input == '\r') {
			mState = expectingNewline_3;
			return boost::indeterminate;
		}
		else if (!request.mHeaders.empty() && (input == ' ' || input == '\t')) {
			mState = headerLws;
			return boost::indeterminate;
		}
		else if (!isChar(input) || isCtl(input) || isTspecial(input)) {
			return false;
		}
		else {
			request.mHeaders.push_back(Header());
			request.mHeaders.back().name.push_back(input);
			mState = headerName;
			return boost::indeterminate;
		}
	case headerLws:
		if (input == '\r') {
			mState = expectingNewline_2;
			return boost::indeterminate;
		}
		else if (input == ' ' || input == '\t') {
			return boost::indeterminate;
		}
		else if (isCtl(input)) {
			return false;
		}
		else {
			mState = headerValue;
			request.mHeaders.back().value.push_back(input);
			return boost::indeterminate;
		}
	case headerName:
		if (input == ':') {
			mState = spaceBeforeHeaderValue;
			return boost::indeterminate;
		}
		else if (!isChar(input) || isCtl(input) || isTspecial(input)) {
			return false;
		}
		else {
			request.mHeaders.back().name.push_back(input);
			return boost::indeterminate;
		}
  case spaceBeforeHeaderValue:
		if (input == ' ') {
			mState = headerValue;
			return boost::indeterminate;
		}
		else {
			return false;
		}
	case headerValue:
		if (input == '\r') {
			mState = expectingNewline_2;
			return boost::indeterminate;
		}
		else if (isCtl(input)) {
			return false;
		}
		else {
			request.mHeaders.back().value.push_back(input);
			return boost::indeterminate;
		}
	case expectingNewline_2:
		if (input == '\n') {
			mState = headerLineStart;
			return boost::indeterminate;
		}
		else {
			return false;
		}
  case expectingNewline_3:
	return (input == '\n');
  default:
	return false;
  }
}

bool RequestParser::isChar(int c)
{
  return c >= 0 && c <= 127;
}

bool RequestParser::isCtl(int c)
{
  return c >= 0 && c <= 31 || c == 127;
}

bool RequestParser::isTspecial(int c)
{
  switch (c)
  {
  case '(': case ')': case '<': case '>': case '@':
  case ',': case ';': case ':': case '\\': case '"':
  case '/': case '[': case ']': case '?': case '=':
  case '{': case '}': case ' ': case '\t':
	return true;
  default:
	return false;
  }
}

bool RequestParser::isDigit(int c)
{
  return c >= '0' && c <= '9';
}
