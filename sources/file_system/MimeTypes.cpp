#include "MimeTypes.hpp"

#include <map>
#include <string>

namespace webserver {

const std::string MimeTypes::DEFAULT_MIME = "application/octet-stream";

std::map<std::string, std::string> MimeTypes::initMimeMap() {
    std::map<std::string, std::string> mimeMap;

    mimeMap["html"] = "text/html";
    mimeMap["css"] = "text/css";
    mimeMap["js"] = "application/javascript";
    mimeMap["png"] = "image/png";
    mimeMap["jpg"] = "image/jpeg";
    mimeMap["jpeg"] = "image/jpeg";
    mimeMap["gif"] = "image/gif";
    mimeMap["mp3"] = "audio/mpeg";
    mimeMap["mp4"] = "video/mp4";
    mimeMap["pdf"] = "application/pdf";
    mimeMap["txt"] = "text/plain";
    mimeMap["ico"] = "image/x-icon";
    mimeMap["json"] = "application/json";
    mimeMap["wasm"] = "application/wasm";
    mimeMap["svg"] = "image/svg+xml";
    mimeMap["webp"] = "image/webp";
    mimeMap["woff"] = "font/woff";
    mimeMap["woff2"] = "font/woff2";
    mimeMap["csv"] = "text/csv";
    mimeMap["xml"] = "application/xml";

    return (mimeMap);
}

std::map<std::string, std::string> MimeTypes::mimeMap = MimeTypes::initMimeMap();

std::string MimeTypes::getMimeType(const std::string& extension) {
    const std::map<std::string, std::string>::const_iterator itr = mimeMap.find(extension);
    if (itr != mimeMap.end()) {
        return (itr->second);
    }
    return (DEFAULT_MIME);
}
}  // namespace webserver
