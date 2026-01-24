#include "MimeType.hpp"

#include <map>
#include <string>

using std::map;
using std::string;

namespace webserver {

const string MimeType::DEFAULT_MIME_TYPE = "application/octet-stream";

MimeType::MimeType()
    : _type(DEFAULT_MIME_TYPE)
    , _isPrintable(false) {
}

MimeType::MimeType(const MimeType& other)
    : _type(other._type)
    , _isPrintable(other._isPrintable) {
}

MimeType::~MimeType() {
}

MimeType::MimeType(const string& type, bool isPrintable)
    : _type(type)
    , _isPrintable(isPrintable) {
}

MimeType& MimeType::operator=(const MimeType& other) {
    if (this == &other) {
        return (*this);
    }
    _type = other._type;
    _isPrintable = other._isPrintable;
    return (*this);
}

map<string, MimeType> MimeType::initMimeTypeMap() {
    map<string, MimeType> map;

    map["html"] = MimeType("text/html", true);
	map["htm"] = map["html"];
    map["css"] = MimeType("text/css", true);
    map["js"] = MimeType("application/javascript", true);
    map["png"] = MimeType("image/png", false);
    map["jpg"] = MimeType("image/jpeg", false);
    map["jpeg"] = map["jpg"];
    map["gif"] = MimeType("image/gif", false);
    map["mp3"] = MimeType("audio/mpeg", false);
    map["mp4"] = MimeType("video/mp4", false);
    map["pdf"] = MimeType("application/pdf", false);
    map["txt"] = MimeType("text/plain", true);
    map["ico"] = MimeType("image/x-icon", false);
    map["json"] = MimeType("application/json", true);
    map["wasm"] = MimeType("application/wasm", false);
    map["svg"] = MimeType("image/svg+xml", false);
    map["webp"] = MimeType("image/webp", false);
    map["woff"] = MimeType("font/woff", false);
    map["woff2"] = MimeType("font/woff2", false);
    map["csv"] = MimeType("text/csv", true);
    map["xml"] = MimeType("application/xml", true);

    return (map);
}

map<string, MimeType> MimeType::_mimeTypeMap = MimeType::initMimeTypeMap();

string MimeType::getMimeType(const string& extension) {
    const map<string, MimeType>::const_iterator itr = _mimeTypeMap.find(extension);
    if (itr != _mimeTypeMap.end()) {
        return (itr->second._type);
    }
    return (DEFAULT_MIME_TYPE);
}

bool MimeType::isPrintable(const string& mimeType) {
    for (map<string, MimeType>::const_iterator itr = _mimeTypeMap.begin();
         itr != _mimeTypeMap.end();
         ++itr) {
        if (itr->second._type == mimeType) {
            return (itr->second._isPrintable);
        }
    }
    return (false);
}
}  // namespace webserver
