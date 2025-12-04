#include "RouteConfig.hpp"

#include <cstddef>

#include "FolderConfig.hpp"
#include "UploadConfig.hpp"

namespace webserver {
RouteConfig::RouteConfig()
    : _folderConfigSection(new FolderConfig("/home/dmlasko/Desktop/webserv", false, ""))
    , _uploadConfigSection(NULL) {
}

RouteConfig::RouteConfig(const RouteConfig& other)
    : _allowedMethods(other._allowedMethods)
    , _redirections(other._redirections)
    , _folderConfigSection(other._folderConfigSection)
    , _uploadConfigSection(other._uploadConfigSection)
    , _cgiHandlers(other._cgiHandlers) {
}

RouteConfig& RouteConfig::operator=(const RouteConfig& other) {
    if (&other == this) {
        return (*this);
    }
    _allowedMethods = other._allowedMethods;
    _redirections = other._redirections;
    _folderConfigSection = new FolderConfig(*other._folderConfigSection);
    _uploadConfigSection = new UploadConfig(*other._uploadConfigSection);
    _cgiHandlers = other._cgiHandlers;
    return (*this);
}

RouteConfig& RouteConfig::setFolderConfig(const FolderConfig& tgt) {
    _folderConfigSection = new FolderConfig(tgt);
    return (*this);
}

const FolderConfig* RouteConfig::getFolderConfig() const {
    return (_folderConfigSection);
}

bool RouteConfig::operator==(const RouteConfig& other) const {
    return (
        _allowedMethods == other._allowedMethods && _redirections == other._redirections &&
        (*_folderConfigSection) == (*(other._folderConfigSection)) &&
        (*_uploadConfigSection) == (*(other._uploadConfigSection)) &&
        _cgiHandlers == other._cgiHandlers
    );
}

RouteConfig::~RouteConfig() {
}
}  // namespace webserver
