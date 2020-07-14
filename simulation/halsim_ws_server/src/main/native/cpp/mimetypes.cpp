

#include "mimetypes.h"

#include <wpi/StringMap.h>

// https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types
static wpi::StringMap<std::string> mimeTypes{

    // text
    {"css", "text/css"},
    {"csv", "text/csv"},
    {"htm", "text/html"},
    {"html", "text/html"},
    {"js", "text/javascript"},
    {"json", "application/json"},
    {"map", "application/json"},
    {"md", "text/markdown"},
    {"txt", "text/plain"},
    {"xml", "text/xml"},

    // images
    {"apng", "image/apng"},
    {"bmp", "image/bmp"},
    {"gif", "image/gif"},
    {"cur", "image/x-icon"},
    {"ico", "image/x-icon"},
    {"jpg", "image/jpeg"},
    {"jpeg", "image/jpeg"},
    {"png", "image/png"},
    {"svg", "image/svg+xml"},
    {"tif", "image/tiff"},
    {"tiff", "image/tiff"},
    {"webp", "image/webp"},

    // fonts
    {"otf", "font/otf"},
    {"ttf", "font/ttf"},
    {"woff", "font/woff"},

    // misc
    {"pdf", "application/pdf"},
    {"zip", "application/zip"},
};

static std::string defaultType("application/octet-stream");

// derived partially from
// https://github.com/DEGoodmanWilson/libmime/blob/stable/0.1.2/mime/mime.cpp
wpi::StringRef MimeTypeFromPath(wpi::StringRef path) {
  auto pos = path.find_last_of("/");
  if (pos != wpi::StringRef::npos) {
    path = path.substr(pos + 1);
  }
  auto dot_pos = path.find_last_of(".");
  if (dot_pos > 0 && dot_pos != wpi::StringRef::npos) {
    auto type = mimeTypes.find(path.substr(dot_pos + 1));
    if (type != mimeTypes.end()) {
      return type->getValue();
    }
  }
  return defaultType;
}