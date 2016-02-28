#include "requestHandler.h"

RequestHandler::RequestHandler(const std::string &docRoot)
	: mDocRoot(docRoot)
{
}

void RequestHandler::handleRequest(Request const &request, Reply &reply)
{
	// Decode url to the normal path.
	std::string requestPath;
	// Check for successful decoding
	if (!urlDecode(request.mUri, requestPath)) {
		reply = Reply::stockReply(Reply::BadRequest);
		return;
	}

	// Request path must be absolute and not contain "..".
	if (requestPath.empty() || requestPath[0] != '/' || requestPath.find("..") != std::string::npos) {
		reply = Reply::stockReply(Reply::BadRequest);
		return;
	}

	// If path ends in slash (i.e. is a directory) then add "index.html".
	if (requestPath[requestPath.size() - 1] == '/') {
		requestPath += "index.html";
	}

	// ? Determine the file extension.
	std::size_t lastSlashPos = requestPath.find_last_of("/");
	std::size_t lastDotPos = requestPath.find_last_of(".");
	std::string extension;
	if (lastDotPos != std::string::npos && lastDotPos > lastSlashPos) {
		extension = requestPath.substr(lastDotPos + 1);
	}

	// Open the file to send back.
	std::string fullPath = mDocRoot + requestPath;
	std::ifstream is(fullPath.c_str(), std::ios::in | std::ios::binary);

	// Check for root path + request path = correct
	if (!is) {
		reply = Reply::stockReply(Reply::NotFound);
		return;
	}

	// Fill out the reply to be sent to the client.
	reply.status = Reply::Ok;
	char buf[512];

	while (is.read(buf, sizeof(buf)).gcount() > 0)
	reply.content.append(buf, is.gcount());

	reply.headers.resize(2);
	reply.headers[0].name = "Content-Length";
	reply.headers[0].value = boost::lexical_cast<std::string>(reply.content.size());
	reply.headers[1].name = "Content-Type";
	reply.headers[1].value = mime::extensionToType(extension);
}

 bool RequestHandler::urlDecode(std::string const &in, std::string& out)
{
	 out.clear();

	// Requests that the string capacity be adapted to a planned change in size to
	// a length of up to n characters.
	 out.reserve(in.size());

	 for (std::size_t i = 0; i < in.size(); ++i) {
		 if (in[i] == '%') {
			 // Translation not latin letters (%D0 -> K)
			 if (i + 3 <= in.size()) {
				 int value = 0;
				 std::istringstream is(in.substr(i + 1, 2));
				 if (is >> std::hex >> value) {
					 out += static_cast<char>(value);
					 i += 2;
				 }
				 else {
					 return false;
				 }
			 }
			 else {
				 return false;
			 }
		 }
		 else if (in[i] == '+') {
			 out += ' ';
		 }
		 else {
			 out += in[i];
		 }
	 }
	 return true;
 }
