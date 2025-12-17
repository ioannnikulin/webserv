#include "configuration/FolderConfig.hpp"

#include <string>

using std::string;

namespace webserver {
FolderConfig::FolderConfig(
    const string& rootPath,
    bool enableListing,
    const string& indexPageFileLocation
)
    : _rootPath(rootPath)
    , _enableListing(enableListing)
    , _indexPageFileLocation(indexPageFileLocation) {
}

FolderConfig::FolderConfig(const FolderConfig& other)
    : _rootPath(other._rootPath)
    , _enableListing(other._enableListing)
    , _indexPageFileLocation(other._indexPageFileLocation) {
}

string FolderConfig::getRootPath() const {
    return (_rootPath);
}

string FolderConfig::getIndexPageFileLocation() const {
    return (_indexPageFileLocation);
}

bool FolderConfig::operator==(const FolderConfig& other) const {
    return (
        _rootPath == other._rootPath && _enableListing == other._enableListing &&
        _indexPageFileLocation == other._indexPageFileLocation
    );
}

FolderConfig::~FolderConfig() {
}
}  // namespace webserver
