#include "RouteConfig.hpp"

#include <cstddef>
#include <cstring>
#include <iostream>
#include <map>
#include <set>
#include <string>

#include "configuration/CgiHandlerConfig.hpp"
#include "configuration/FolderConfig.hpp"
#include "configuration/UploadConfig.hpp"
#include "http_methods/HttpMethodType.hpp"

using std::map;
using std::ostream;
using std::set;
using std::string;

namespace webserver {
RouteConfig::RouteConfig()
    : _path("")
    , _allowedMethods()
    , _isRedirection(false)
    , _redirectTo("")
    , _folderConfigSection()
    , _uploadConfigSection()
    , _cgiHandlers() {
}

RouteConfig::RouteConfig(const RouteConfig& other)
    : _path(other._path)
    , _allowedMethods(other._allowedMethods)
    , _isRedirection(other._isRedirection)
    , _redirectTo(other._redirectTo)
    , _folderConfigSection(other._folderConfigSection)
    , _uploadConfigSection(other._uploadConfigSection) {
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
    _isRedirection = other._isRedirection;
    _redirectTo = other._redirectTo;
    _folderConfigSection = other._folderConfigSection;
    _uploadConfigSection = other._uploadConfigSection;
    _cgiHandlers = other._cgiHandlers;

    for (std::map<std::string, CgiHandlerConfig*>::iterator it = _cgiHandlers.begin();
         it != _cgiHandlers.end();
         ++it) {
        delete it->second;
    }
    _cgiHandlers.clear();
    _folderConfigSection = FolderConfig(other._folderConfigSection);
    _uploadConfigSection = UploadConfig(other._uploadConfigSection);

    for (std::map<std::string, CgiHandlerConfig*>::const_iterator it = other._cgiHandlers.begin();
         it != other._cgiHandlers.end();
         ++it) {
        _cgiHandlers[it->first] = new CgiHandlerConfig(*it->second);
    }
    return (*this);
}

RouteConfig& RouteConfig::setFolderConfig(const FolderConfig& folder) {
    _folderConfigSection = FolderConfig(folder);
    return (*this);
}

const FolderConfig& RouteConfig::getFolderConfig() const {
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
    if (_isRedirection != other._isRedirection) {
        return (false);
    }
    if (_redirectTo != other._redirectTo) {
        return (false);
    }

    if (_folderConfigSection != other._folderConfigSection) {
        return (false);
    }

    if (_uploadConfigSection != other._uploadConfigSection) {
        return (false);
    }
    if (!compareCgiHandlers(other)) {
        return (false);
    }
    return (true);
}

bool RouteConfig::operator<(const RouteConfig& other) const {
    return (_path < other._path);
}

bool RouteConfig::isMethodAllowed(HttpMethodType method) const {
    return (_allowedMethods.find(method) != _allowedMethods.end());
}

RouteConfig::~RouteConfig() {
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

const UploadConfig& RouteConfig::getUploadConfigSection() const {
    return (_uploadConfigSection);
}

RouteConfig& RouteConfig::setUploadConfig(const UploadConfig& upload) {
    _uploadConfigSection = UploadConfig(upload);
    return (*this);
}

RouteConfig& RouteConfig::addAllowedMethod(HttpMethodType method) {
    _allowedMethods.insert(method);
    return (*this);
}

RouteConfig& RouteConfig::setRedirection(const string& redirectTo) {
    _isRedirection = true;
    _redirectTo = redirectTo;
    return (*this);
}

bool RouteConfig::isRedirection() const {
    return (_isRedirection);
}

const string& RouteConfig::getRedirection() const {
    return (_redirectTo);
}

RouteConfig& RouteConfig::addCgiHandler(const CgiHandlerConfig& cfg, string extension) {
    _cgiHandlers[extension] = new CgiHandlerConfig(cfg);
    return (*this);
}

const std::map<std::string, CgiHandlerConfig*>& RouteConfig::getCgiHandlers() const {
    return (_cgiHandlers);
}

string RouteConfig::getPath() const {
    return (_path);
}

ostream& operator<<(ostream& oss, const RouteConfig& route) {
    oss << route._path;
    oss << "\n";
    for (set<HttpMethodType>::const_iterator itr = route._allowedMethods.begin();
         itr != route._allowedMethods.end();
         itr++) {
        oss << methodToString(*itr) << " ";
    }
    oss << "\n";
    oss << route._isRedirection << " " << route._redirectTo << "\n";
    oss << route._folderConfigSection;
    oss << route._uploadConfigSection;
    oss << "\n";
    for (map<string, CgiHandlerConfig*>::const_iterator itr = route._cgiHandlers.begin();
         itr != route._cgiHandlers.end();
         itr++) {
        oss << itr->first << "->" << *itr->second << "\n";
    }
    return (oss);
}
}  // namespace webserver
