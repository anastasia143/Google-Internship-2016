#include "mimeTypes.h"

namespace mime {

struct Mapping
{
	const char* extension;
	const char* mimeType;
} mappings[] =
{
	{ "gif", "image/gif" },
	{ "htm", "text/html" },
	{ "html", "text/html" },
	{ "jpg", "image/jpeg" },
	{ "png", "image/png" },
	{ 0, 0 } // Marks end of list.
};

/// Convert a file extension into a MIME type.
std::string extensionToType(std::string const &extension)
{
	for (Mapping* m = mappings; m->extension; ++m) {
		if (m->extension == extension) {
			return m->mimeType;
		}
	}
	return "text/plain";
}

}

