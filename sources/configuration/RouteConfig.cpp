#include "RouteConfig.hpp"

#include <iostream>
#include <map>
#include <set>
#include <string>

#include "configuration/CgiHandlerConfig.hpp"
#include "configuration/FolderConfig.hpp"
#include "configuration/UploadConfig.hpp"
#include "http_methods/HttpMethodType.hpp"

using std::endl;
using std::map;
using std::ostream;
using std::set;
using std::string;

namespace webserver {
RouteConfig::RouteConfig() {
}

RouteConfig::RouteConfig(const RouteConfig& other)
    : _path(other._path)
    , _allowedMethods(other._allowedMethods)
    , _redirections(other._redirections)
    , _folderConfigSection(other._folderConfigSection)
    , _uploadConfigSection(other._uploadConfigSection)
    , _cgiHandlers(other._cgiHandlers) {
}

RouteConfig& RouteConfig::operator=(const RouteConfig& other) {
    if (&other == this) {
        return (*this);
    }
    _path = other._path;
    _allowedMethods = other._allowedMethods;
    _redirections = other._redirections;
    _folderConfigSection = other._folderConfigSection;
    _uploadConfigSection = other._uploadConfigSection;
    _cgiHandlers = other._cgiHandlers;
    return (*this);
}

RouteConfig& RouteConfig::setFolderConfig(const FolderConfig& folder) {
    _folderConfigSection = FolderConfig(folder);
    return (*this);
}

const FolderConfig& RouteConfig::getFolderConfig() const {
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

    if (_folderConfigSection != other._folderConfigSection) {
        return (false);
    }

    if (_uploadConfigSection != other._uploadConfigSection) {
        return (false);
    }

    if (_cgiHandlers != other._cgiHandlers) {
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
}

RouteConfig& RouteConfig::setPath(string path) {
    _path = path;
    return (*this);
}

const UploadConfig& RouteConfig::getUploadConfig() const {
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

ostream& operator<<(ostream& oss, const RouteConfig& route) {
    oss << route._path;
    oss << endl;
    for (set<HttpMethodType>::const_iterator itr = route._allowedMethods.begin();
         itr != route._allowedMethods.end();
         itr++) {
        oss << methodToString(*itr) << " ";
    }
    oss << endl;
    for (map<string, string>::const_iterator itr = route._redirections.begin();
         itr != route._redirections.end();
         itr++) {
        oss << itr->first << "->" << itr->second << endl;
    }
    oss << route._folderConfigSection;
    oss << route._uploadConfigSection;
    oss << endl;
    for (map<string, CgiHandlerConfig>::const_iterator itr = route._cgiHandlers.begin();
         itr != route._cgiHandlers.end();
         itr++) {
        oss << itr->first << "->" << itr->second << endl;
    }
    return (oss);
}
}  // namespace webserver
