#include "configuration/FolderConfig.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>

#include "logger/Logger.hpp"

using std::ostream;
using std::string;

namespace webserver {
Logger FolderConfig::_log;

size_t FolderConfig::defaultMaxClientBodySizeBytes() {
    // NOTE: cannot be a static constant field due to initialization order problems
    return (std::numeric_limits<std::streamsize>::max());
}

FolderConfig::FolderConfig()
    : _requestedLocation("")
    , _storageRootPath("")
    , _enableListing(false)
    , _indexPageFilename("")
    , _maxClientBodySizeBytes(defaultMaxClientBodySizeBytes()) {
}

FolderConfig::FolderConfig(
    const string& location,
    const string& rootPath,
    bool enableListing,
    const string& indexPageFilename,
    size_t maxClientBodySizeBytes
)
    : _requestedLocation(location)
    , _storageRootPath(rootPath)
    , _enableListing(enableListing)
    , _indexPageFilename(indexPageFilename)
    , _maxClientBodySizeBytes(maxClientBodySizeBytes) {
}

FolderConfig::FolderConfig(const FolderConfig& other)
    : _requestedLocation(other._requestedLocation)
    , _storageRootPath(other._storageRootPath)
    , _enableListing(other._enableListing)
    , _indexPageFilename(other._indexPageFilename)
    , _maxClientBodySizeBytes(other._maxClientBodySizeBytes) {
}

string FolderConfig::getRootPath() const {
    return (_storageRootPath);
}

string FolderConfig::getResolvedPath(std::string target) const {
    if (target.empty()) {
        target = "/";
    }

    if (target == "/set_light" || target == "/set_dark") {
        return (target);
    }
    _log.stream(LOG_DEBUG) << "[" << _storageRootPath << "] [" << target << "]\n";
    return (
        _storageRootPath + "/" +
        target.substr(_requestedLocation.length(), target.length() - _requestedLocation.length())
    );
}

string FolderConfig::getIndexPageFilename() const {
    return (_indexPageFilename);
}

bool FolderConfig::isListingEnabled() const {
    return (_enableListing);
}

bool FolderConfig::operator==(const FolderConfig& other) const {
    return (
        _requestedLocation == other._requestedLocation &&
        _storageRootPath == other._storageRootPath && _enableListing == other._enableListing &&
        _indexPageFilename == other._indexPageFilename &&
        _maxClientBodySizeBytes == other._maxClientBodySizeBytes
    );
}

bool FolderConfig::doesLocationBlockServeFiles() const {
    if (!_storageRootPath.empty()) {
        return (true);
    }
    if (!_indexPageFilename.empty()) {
        return (true);
    }
    if (_enableListing) {
        return (true);
    }
    return (false);
}

void FolderConfig::setRootPath(std::string root) {
    _storageRootPath = root;
}

bool FolderConfig::operator!=(const FolderConfig& other) const {
    return (!(*this == other));
}

FolderConfig& FolderConfig::operator=(const FolderConfig& other) {
    if (this == &other) {
        return (*this);
    }
    _requestedLocation = other._requestedLocation;
    _storageRootPath = other._storageRootPath;
    _enableListing = other._enableListing;
    _indexPageFilename = other._indexPageFilename;
    _maxClientBodySizeBytes = other._maxClientBodySizeBytes;
    return (*this);
}

size_t FolderConfig::getMaxClientBodySizeBytes() const {
    return (_maxClientBodySizeBytes);
}

FolderConfig::~FolderConfig() {
}

ostream& operator<<(ostream& oss, const FolderConfig& config) {
    oss << config._requestedLocation;
    oss << " " << config._storageRootPath;
    oss << " " << config._enableListing;
    oss << " " << config._indexPageFilename;
    oss << " " << config._maxClientBodySizeBytes;
    oss << "\n";
    return (oss);
}
}  // namespace webserver
