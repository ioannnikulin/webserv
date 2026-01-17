
#include "configuration/parser/LocationTempData.hpp"

#include <cstddef>
#include <string>

using std::string;
namespace webserver {
LocationTempData::LocationTempData()
    : _rootSet(false)
    , _autoindex(false)
    , _autoindexSet(false)
    , _indexSet(false)
    , _uploadEnabled(false)
    , _uploadSet(false)
    , _maxClientBodySizeBytesSet(false) {
}

LocationTempData::~LocationTempData() {
}

void LocationTempData::clear() {
    _rootSet = false;
    _storageRootPath = "";

    _autoindexSet = false;
    _autoindex = false;

    _indexSet = false;
    _indexPage = "";

    _uploadSet = false;
    _uploadEnabled = false;
    _uploadRoot = "";

    _maxClientBodySizeBytesSet = false;
}

void LocationTempData::setRootPath(const string& path) {
    _storageRootPath = path;
    _rootSet = true;
}
const string& LocationTempData::rootPath() const {
    return (_storageRootPath);
}
bool LocationTempData::rootSet() const {
    return (_rootSet);
}

void LocationTempData::setAutoindex(bool value) {
    _autoindex = value;
    _autoindexSet = true;
}

bool LocationTempData::autoindex() const {
    return (_autoindex);
}

bool LocationTempData::autoindexSet() const {
    return (_autoindexSet);
}

void LocationTempData::setIndexPage(const string& page) {
    _indexPage = page;
    _indexSet = true;
}

const string& LocationTempData::indexPage() const {
    return (_indexPage);
}

bool LocationTempData::indexSet() const {
    return (_indexSet);
}

void LocationTempData::setUploadEnabled(bool value) {
    _uploadEnabled = value;
}

bool LocationTempData::uploadEnabled() const {
    return (_uploadEnabled);
}

void LocationTempData::setUploadRoot(const string& path) {
    _uploadRoot = path;
    _uploadSet = true;
}

const string& LocationTempData::uploadRoot() const {
    return (_uploadRoot);
}

bool LocationTempData::uploadSet() const {
    return (_uploadSet);
}

void LocationTempData::setMaxClientBodySizeBytes(size_t maxBodySizeBytes) {
    _maxClientBodySizeBytes = maxBodySizeBytes;
    _maxClientBodySizeBytesSet = true;
}

size_t LocationTempData::getMaxBodySizeBytes() const {
    return (_maxClientBodySizeBytes);
}

bool LocationTempData::maxBodySizeBytesSet() const {
    return (_maxClientBodySizeBytesSet);
}

}  // namespace webserver
