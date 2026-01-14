#include "RouteConfig.hpp"

#include <cstddef>
#include <map>
#include <string>

#include "configuration/CgiHandlerConfig.hpp"
#include "configuration/FolderConfig.hpp"
#include "configuration/UploadConfig.hpp"
#include "http_methods/HttpMethodType.hpp"

using std::string;
namespace webserver {
RouteConfig::RouteConfig()
    : _folderConfigSection(NULL)
    , _uploadConfigSection(NULL) {
}

RouteConfig::RouteConfig(const RouteConfig& other)
    : _path(other._path)
    , _allowedMethods(other._allowedMethods)
    , _redirections(other._redirections)
    , _folderConfigSection(NULL)
    , _uploadConfigSection(NULL) {
    if (other._folderConfigSection != NULL) {
        _folderConfigSection = new FolderConfig(*other._folderConfigSection);
    }
    if (other._uploadConfigSection != NULL) {
        _uploadConfigSection = new UploadConfig(*other._uploadConfigSection);
    }
    for (std::map<std::string, CgiHandlerConfig*>::const_iterator it = other._cgiHandlers.begin();
         it != other._cgiHandlers.end();
         ++it) {
        _cgiHandlers[it->first] = new CgiHandlerConfig(*it->second);
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

    for (std::map<std::string, CgiHandlerConfig*>::iterator it = _cgiHandlers.begin();
         it != _cgiHandlers.end();
         ++it) {
        delete it->second;
    }
    _cgiHandlers.clear();
    _folderConfigSection =
        (other._folderConfigSection != NULL) ? new FolderConfig(*other._folderConfigSection) : NULL;
    _uploadConfigSection =
        (other._uploadConfigSection != NULL) ? new UploadConfig(*other._uploadConfigSection) : NULL;

    for (std::map<std::string, CgiHandlerConfig*>::const_iterator it = other._cgiHandlers.begin();
         it != other._cgiHandlers.end();
         ++it) {
        _cgiHandlers[it->first] = new CgiHandlerConfig(*it->second);
    }
    return (*this);
}

RouteConfig& RouteConfig::setFolderConfig(const FolderConfig& folder) {
    delete _folderConfigSection;
    _folderConfigSection = new FolderConfig(folder);
    return (*this);
}

const FolderConfig* RouteConfig::getFolderConfig() const {
    return (_folderConfigSection);
}

FolderConfig* RouteConfig::getFolderConfig() {
    return (_folderConfigSection);
}

bool RouteConfig::compareCgiHandlers(const RouteConfig& other) const {
    if (_cgiHandlers.size() != other._cgiHandlers.size()) {
        return (false);
    }
    for (std::map<std::string, CgiHandlerConfig*>::const_iterator it = _cgiHandlers.begin();
         it != _cgiHandlers.end();
         ++it) {
        const std::map<std::string, CgiHandlerConfig*>::const_iterator otherIt =
            other._cgiHandlers.find(it->first);
        if (otherIt == other._cgiHandlers.end()) {
            return (false);
        }
        if (it->second == NULL && otherIt->second == NULL) {
            continue;
        }
        if (it->second == NULL || otherIt->second == NULL) {
            return (false);
        }
        if (!(*it->second == *otherIt->second)) {
            return (false);
        }
    }
    return (true);
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
    if (!compareCgiHandlers(other)) {
        return (false);
    }
    return (true);
}

bool RouteConfig::operator<(const RouteConfig& other) const {
    return (_path < other._path);
}

RouteConfig::~RouteConfig() {
    delete _folderConfigSection;
    delete _uploadConfigSection;

    for (std::map<std::string, CgiHandlerConfig*>::iterator it = _cgiHandlers.begin();
         it != _cgiHandlers.end();
         ++it) {
        delete it->second;
    }
}

RouteConfig& RouteConfig::setPath(string path) {
    _path = path;
    return (*this);
}

RouteConfig& RouteConfig::setUploadConfig(const UploadConfig& upload) {
    delete _uploadConfigSection;
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
    _cgiHandlers[extension] = new CgiHandlerConfig(cfg);
    return (*this);
}

UploadConfig* RouteConfig::getUploadConfigSection() const {
    return (_uploadConfigSection);
}

const std::map<std::string, CgiHandlerConfig*>& RouteConfig::getCgiHandlers() const {
    return (_cgiHandlers);
}

string RouteConfig::getPath() const {
    return (_path);
}
}  // namespace webserver
