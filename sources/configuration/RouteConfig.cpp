#include "RouteConfig.hpp"

#include <cstddef>
#include <string>

#include "configuration/CgiHandlerConfig.hpp"
#include "configuration/FolderConfig.hpp"
#include "configuration/UploadConfig.hpp"
#include "http_methods/HttpMethodType.hpp"

using std::string;
namespace webserver {
RouteConfig::RouteConfig()
    : _folderConfigSection(new FolderConfig("/home/dmlasko/Desktop/webserv", false, ""))
    , _uploadConfigSection(NULL) {
}

RouteConfig::RouteConfig(const RouteConfig& other)
    : _path(other._path)
    , _allowedMethods(other._allowedMethods)
    , _redirections(other._redirections)
    , _folderConfigSection(NULL)
    , _uploadConfigSection(NULL)
    , _cgiHandlers(other._cgiHandlers) {
    if (other._folderConfigSection != NULL) {
        _folderConfigSection = new FolderConfig(*other._folderConfigSection);
    }
    if (other._uploadConfigSection != NULL) {
        _uploadConfigSection = new UploadConfig(*other._uploadConfigSection);
    }
}

RouteConfig& RouteConfig::operator=(const RouteConfig& other) {
    if (&other == this) {
        return (*this);
    }
    _path = other._path;
    _allowedMethods = other._allowedMethods;
    _redirections = other._redirections;
    _cgiHandlers = other._cgiHandlers;

    delete _folderConfigSection;
    delete _uploadConfigSection;

    _folderConfigSection =
        (other._folderConfigSection != NULL) ? new FolderConfig(*other._folderConfigSection) : NULL;
    _uploadConfigSection =
        (other._uploadConfigSection != NULL) ? new UploadConfig(*other._uploadConfigSection) : NULL;

    return (*this);
}

RouteConfig& RouteConfig::setFolderConfig(const FolderConfig& folder) {
    _folderConfigSection = new FolderConfig(folder);
    return (*this);
}

const FolderConfig* RouteConfig::getFolderConfig() const {
    return (_folderConfigSection);
}

bool RouteConfig::operator==(const RouteConfig& other) const {
    if (_path != other._path) {
        return (false);
    }

    if (_allowedMethods != other._allowedMethods) {
        return (false);
    }

    if (_redirections != other._redirections) {
        return (false);
    }

    if (_folderConfigSection == NULL && other._folderConfigSection == NULL) {
    } else if (_folderConfigSection == NULL || other._folderConfigSection == NULL) {
        return (false);
    } else {
        if (!(*_folderConfigSection == *other._folderConfigSection)) {
            return (false);
        }
    }

    if (_uploadConfigSection == NULL && other._uploadConfigSection == NULL) {
    } else if (_uploadConfigSection == NULL || other._uploadConfigSection == NULL) {
        return (false);
    } else {
        if (!(*_uploadConfigSection == *other._uploadConfigSection)) {
            return (false);
        }
    }

    if (_cgiHandlers != other._cgiHandlers) {
        return (false);
    }

    return (true);
}

RouteConfig::~RouteConfig() {
}

RouteConfig& RouteConfig::setPath(string path) {
    _path = path;
    return (*this);
}

RouteConfig& RouteConfig::setUploadConfig(const UploadConfig& upload) {
    _uploadConfigSection = new UploadConfig(upload);
    return (*this);
}

RouteConfig& RouteConfig::addAllowedMethod(HttpMethodType method) {
    _allowedMethods.insert(method);
    return (*this);
}

RouteConfig& RouteConfig::addRedirection(const string& from, const string& toDir) {
    _redirections[from] = toDir;
    return (*this);
}

RouteConfig& RouteConfig::addCgiHandler(const CgiHandlerConfig& cfg, string extension) {
    _cgiHandlers[extension] = cfg;
    return (*this);
}

string RouteConfig::getPath() const {
    return (_path);
}
}  // namespace webserver
