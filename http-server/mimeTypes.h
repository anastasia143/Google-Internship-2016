#pragma once

#include <string>

namespace mime {

/// Convert a file extension into a MIME type.
std::string extensionToType(std::string const &extension);

}
