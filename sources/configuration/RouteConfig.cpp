#include "RouteConfig.hpp"

#include <cstddef>
#include <cstring>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>

#include "configuration/CgiHandlerConfig.hpp"
#include "configuration/FolderConfig.hpp"
#include "configuration/UploadConfig.hpp"
#include "configuration/parser/ConfigParsingException.hpp"
#include "http_methods/HttpMethodType.hpp"
#include "http_status/HttpStatus.hpp"
#include "logger/Logger.hpp"

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
    , _cgiHandlers()
    , _statusCatalogue() {
}

RouteConfig::RouteConfig(const RouteConfig& other)
    : _path(other._path)
    , _allowedMethods(other._allowedMethods)
    , _isRedirection(other._isRedirection)
    , _redirectTo(other._redirectTo)
    , _folderConfigSection(other._folderConfigSection)
    , _uploadConfigSection(other._uploadConfigSection)
    , _statusCatalogue(other._statusCatalogue) {
    for (std::map<std::string, CgiHandlerConfig*>::const_iterator it = other._cgiHandlers.begin();
         it != other._cgiHandlers.end();
         ++it) {
        _cgiHandlers[it->first] = (it->second == NULL ? NULL : new CgiHandlerConfig(*it->second));
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
    _statusCatalogue = other._statusCatalogue;

    for (std::map<std::string, CgiHandlerConfig*>::iterator it = _cgiHandlers.begin();
         it != _cgiHandlers.end();
         ++it) {
        delete it->second;
    }
    _cgiHandlers.clear();

    for (std::map<std::string, CgiHandlerConfig*>::const_iterator it = other._cgiHandlers.begin();
         it != other._cgiHandlers.end();
         ++it) {
        _cgiHandlers[it->first] = (it->second == NULL ? NULL : new CgiHandlerConfig(*it->second));
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
    if (_statusCatalogue != other._statusCatalogue) {
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
    const std::pair<std::set<HttpMethodType>::iterator, bool> result =
        _allowedMethods.insert(method);
    if (!result.second) {
        throw ConfigParsingException("Duplicate HTTP method in location block");
    }
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

RouteConfig& RouteConfig::setStatusCatalogue(const HttpStatus& statusCatalogue) {
    Logger log;
    log.stream(LOG_TRACE) << "RouteConfig " << this << " set statusCatalogue = " << &statusCatalogue
                          << "\n";
    _statusCatalogue = statusCatalogue;
    return (*this);
}

const HttpStatus& RouteConfig::getStatusCatalogue() const {
    return (_statusCatalogue);
}

const std::string& RouteConfig::getStatusPageFileLocation(HttpStatus::CODE code) const {
    return (_statusCatalogue.getPageFileLocation(code));
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
