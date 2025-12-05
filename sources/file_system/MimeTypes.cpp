#include "MimeTypes.hpp"

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

    return (mimeMap);
}

const std::map<std::string, std::string> MimeTypes::mimeMap = MimeTypes::initMimeMap();

std::string MimeTypes::getMimeType(const std::string& extension) {
    std::map<std::string, std::string>::const_iterator it = mimeMap.find(extension);
    if (it != mimeMap.end()) {
        return it->second;
    }
    return "application/octet-stream";
}
