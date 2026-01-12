#include "configuration/FolderConfig.hpp"

#include <string>

#include "logger/Logger.hpp"

using std::string;

namespace webserver {

Logger FolderConfig::_log;

FolderConfig::FolderConfig(
    const string& location,
    const string& rootPath,
    bool enableListing,
    const string& indexPageFilename
)
    : _requestedLocation(location)
    , _storageRootPath(rootPath)
    , _enableListing(enableListing)
    , _indexPageFilename(indexPageFilename) {
}

FolderConfig::FolderConfig(const FolderConfig& other)
    : _requestedLocation(other._requestedLocation)
    , _storageRootPath(other._storageRootPath)
    , _enableListing(other._enableListing)
    , _indexPageFilename(other._indexPageFilename) {
}

string FolderConfig::getRootPath() const {
    return (_storageRootPath);
}

string FolderConfig::getResolvedPath(std::string target) const {
    if (target.empty()) {
        target = "/";
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
        _storageRootPath == other._storageRootPath && _enableListing == other._enableListing &&
        _indexPageFilename == other._indexPageFilename
    );
}

FolderConfig::~FolderConfig() {
}
}  // namespace webserver
