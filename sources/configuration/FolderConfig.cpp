#include "FolderConfig.hpp"

#include <string>

namespace webserver {
FolderConfig::FolderConfig(
    const std::string& rootPath,
    bool enableListing,
    const std::string& indexPageFileLocation
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

std::string FolderConfig::getRootPath() const {
    return (_rootPath);
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
