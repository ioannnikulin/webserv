#include "MimeTypes.hpp"

#include <map>
#include <string>

namespace webserver {

const std::string MimeTypes::DEFAULT_MIME_TYPE = "application/octet-stream";

std::map<std::string, std::string> MimeTypes::initMimeTypeMap() {
    std::map<std::string, std::string> map;

    map["html"] = "text/html";
    map["css"] = "text/css";
    map["js"] = "application/javascript";
    map["png"] = "image/png";
    map["jpg"] = "image/jpeg";
    map["jpeg"] = "image/jpeg";
    map["gif"] = "image/gif";
    map["mp3"] = "audio/mpeg";
    map["mp4"] = "video/mp4";
    map["pdf"] = "application/pdf";
    map["txt"] = "text/plain";
    map["ico"] = "image/x-icon";
    map["json"] = "application/json";
    map["wasm"] = "application/wasm";
    map["svg"] = "image/svg+xml";
    map["webp"] = "image/webp";
    map["woff"] = "font/woff";
    map["woff2"] = "font/woff2";
    map["csv"] = "text/csv";
    map["xml"] = "application/xml";

    return (map);
}

std::map<std::string, std::string> MimeTypes::_mimeTypeMap = MimeTypes::initMimeTypeMap();

std::string MimeTypes::getMimeType(const std::string& extension) {
    const std::map<std::string, std::string>::const_iterator itr = _mimeTypeMap.find(extension);
    if (itr != _mimeTypeMap.end()) {
        return (itr->second);
    }
    return (DEFAULT_MIME_TYPE);
}
}  // namespace webserver
