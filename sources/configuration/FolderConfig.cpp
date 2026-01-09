#include "configuration/FolderConfig.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>

using std::string;

namespace webserver {
const size_t FolderConfig::DEFAULT_MAX_CLIENT_BODY_SIZE_BYTES =
    std::numeric_limits<std::streamsize>::max();

FolderConfig::FolderConfig()
    : _enableListing(false)
    , _maxClientBodySizeBytes(DEFAULT_MAX_CLIENT_BODY_SIZE_BYTES) {
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
    std::cout << "[" << _storageRootPath << "] [" << target << "]" << std::endl;
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
        _requestedLocation == other._requestedLocation && _storageRootPath == other._storageRootPath && _enableListing == other._enableListing &&
        _indexPageFilename == other._indexPageFilename &&
        _maxClientBodySizeBytes == other._maxClientBodySizeBytes
    );
}

bool FolderConfig::operator!=(const FolderConfig& other) const {
    return (!(*this == other));
}

FolderConfig& FolderConfig::operator=(const FolderConfig& other) {
    if (*this == other) {
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
}  // namespace webserver
