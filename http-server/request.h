#pragma once

#include <string>
#include <vector>

#include "header.h"

/// A request received from a client.
class Request
{
public:
	std::string mMethod;
	std::string mUri;
	int httpVersionMajor;
	int httpVersionMinor;
	std::vector<Header> mHeaders;
};
