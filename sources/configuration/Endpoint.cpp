#include "Endpoint.hpp"

#include <cstddef>
#include <map>
#include <set>
#include <stdexcept>
#include <string>

#include "configuration/CgiHandlerConfig.hpp"
#include "configuration/RouteConfig.hpp"
#include "configuration/UploadConfig.hpp"

using std::map;
using std::set;
using std::string;

namespace webserver {
const string Endpoint::DEFAULT_ROOT;
const string Endpoint::DEFAULT_INTERFACE = "127.0.0.1";
const int Endpoint::DEFAULT_PORT = 8888;
const unsigned long Endpoint::DEFAULT_MAX_BODY_SIZE_BYTES = 1 << 20;

Endpoint::Endpoint()
    : _interface(DEFAULT_INTERFACE)
    , _port(DEFAULT_PORT)
    , _rootDirectory(DEFAULT_ROOT)
    , _maxRequestBodySizeBytes(DEFAULT_MAX_BODY_SIZE_BYTES)
    , _uploadConfig(NULL) {
}

Endpoint::Endpoint(const std::string& interface, int port)
    : _interface(interface)
    , _port(port)
    , _rootDirectory(DEFAULT_ROOT)
    , _maxRequestBodySizeBytes(DEFAULT_MAX_BODY_SIZE_BYTES)
    , _uploadConfig(NULL) {
}

Endpoint::Endpoint(const Endpoint& other)
    : _interface(other._interface)
    , _port(other._port)
    , _serverName(other._serverName)
    , _rootDirectory(other._rootDirectory)
    , _maxRequestBodySizeBytes(other._maxRequestBodySizeBytes)
    , _routes(other._routes)
    , _uploadConfig(NULL) {
    for (map<std::string, CgiHandlerConfig*>::const_iterator it = other._cgiHandlers.begin();
         it != other._cgiHandlers.end();
         ++it) {
        _cgiHandlers[it->first] = new CgiHandlerConfig(*it->second);
    }
    if (other._uploadConfig != NULL) {
        _uploadConfig = new UploadConfig(*other._uploadConfig);
    }
}

Endpoint& Endpoint::operator=(const Endpoint& other) {
    if (this == &other) {
        return (*this);
    }

    // NOTE: cleanup

    for (map<std::string, CgiHandlerConfig*>::iterator it = _cgiHandlers.begin();
         it != _cgiHandlers.end();
         ++it) {
        delete it->second;
    }
    _cgiHandlers.clear();

    delete _uploadConfig;
    _uploadConfig = NULL;

    // NOTE: copying

    for (map<std::string, CgiHandlerConfig*>::const_iterator it = other._cgiHandlers.begin();
         it != other._cgiHandlers.end();
         ++it) {
        _cgiHandlers[it->first] = new CgiHandlerConfig(*it->second);
    }

    _uploadConfig = (other._uploadConfig != NULL) ? new UploadConfig(*other._uploadConfig) : NULL;

    _interface = other._interface;
    _port = other._port;
    _serverName = other._serverName;
    _rootDirectory = other._rootDirectory;
    _maxRequestBodySizeBytes = other._maxRequestBodySizeBytes;
    _routes = other._routes;

    return (*this);
}

int Endpoint::getPort(void) const {
    return (_port);
}

string Endpoint::getInterface(void) const {
    return (_interface);
}

bool Endpoint::operator<(const Endpoint& other) const {
    if (_interface != other._interface) {
        return (_interface < other._interface);
    }
    return (_port < other._port);
}

bool Endpoint::operator==(const Endpoint& other) const {
    if (_interface != other._interface) {
        return (false);
    }
    if (_port != other._port) {
        return (false);
    }
    if (_serverName != other._serverName) {
        return (false);
    }
    if (_rootDirectory != other._rootDirectory) {
        return (false);
    }
    if (_maxRequestBodySizeBytes != other._maxRequestBodySizeBytes) {
        return (false);
    }

    if (_cgiHandlers.size() != other._cgiHandlers.size()) {
        return (false);
    }

    map<std::string, CgiHandlerConfig*>::const_iterator it1 = _cgiHandlers.begin();
    map<std::string, CgiHandlerConfig*>::const_iterator it2 = other._cgiHandlers.begin();

    while (it1 != _cgiHandlers.end()) {
        if (it1->first != it2->first) {
            return (false);
        }
        if ((it1->second == NULL) != (it2->second == NULL)) {
            return (false);
        }
        if (it1->second != NULL && it2->second != NULL) {
            if (!(*(it1->second) == *(it2->second))) {
                return (false);
            }
        }

        ++it1;
        ++it2;
    }
    if (_routes != other._routes) {
        return (false);
    }

    if (_uploadConfig == NULL && other._uploadConfig == NULL) {
    } else if (_uploadConfig == NULL || other._uploadConfig == NULL) {
        return (false);
    } else {
        if (!(*_uploadConfig == *other._uploadConfig)) {
            return (false);
        }
    }

    return (true);
}

Endpoint::~Endpoint() {
    delete _uploadConfig;
    for (map<std::string, CgiHandlerConfig*>::iterator it = _cgiHandlers.begin();
         it != _cgiHandlers.end();
         ++it) {
        delete it->second;
    }
    _cgiHandlers.clear();
}

Endpoint& Endpoint::setInterface(string interface) {
    _interface = interface;
    return (*this);
}

Endpoint& Endpoint::setPort(const int& port) {
    _port = port;
    return (*this);
}

Endpoint& Endpoint::addServerName(const string& name) {
    _serverName = name;
    return (*this);
}

Endpoint& Endpoint::setRoot(const string& path) {
    _rootDirectory = path;
    return (*this);
}

Endpoint& Endpoint::setClientMaxBodySizeBytes(size_t size) {
    _maxRequestBodySizeBytes = size;
    return (*this);
}

Endpoint& Endpoint::addCgiHandler(const CgiHandlerConfig& config, string extension) {
    _cgiHandlers[extension] = new CgiHandlerConfig(config);
    return (*this);
}

Endpoint& Endpoint::addRoute(const RouteConfig& route) {
    _routes.insert(route);
    return (*this);
}

set<RouteConfig> Endpoint::getRoutes() const {
    return (_routes);
}

UploadConfig* Endpoint::getUploadConfig() const {
    return (_uploadConfig);
}

string Endpoint::getRoot() const {
    return (_rootDirectory);
}

RouteConfig Endpoint::getRoute(std::string route) const {
    for (std::set<RouteConfig>::const_iterator it = _routes.begin(); it != _routes.end(); ++it) {
        if (it->getPath() == route) {
            return (*it);
        }
    }
    throw std::out_of_range("Route not found");
}

RouteConfig Endpoint::selectRoute(std::string route) const {
    RouteConfig bestMatch;
    size_t bestLength = 0;
    for (std::set<RouteConfig>::const_iterator itr = _routes.begin(); itr != _routes.end(); ++itr) {
        if (route.substr(0, itr->getPath().length()) == itr->getPath()) {
            if (itr->getPath().length() > bestLength) {
                bestMatch = *itr;
                bestLength = itr->getPath().length();
            }
        }
    }
    if (bestLength == 0) {
        throw std::out_of_range("Route not found; is there a root location in the configuration?");
    }
    return (bestMatch);
}

Endpoint& Endpoint::setUploadConfig(const UploadConfig& cfg) {
    delete _uploadConfig;
    _uploadConfig = new UploadConfig(cfg);
    return (*this);
}

const std::map<std::string, CgiHandlerConfig*>& Endpoint::getCgiHandlers() const {
    return (_cgiHandlers);
}

bool Endpoint::isAValidPort(int port) {
    return (port >= MIN_PORT && port <= MAX_PORT);
}
}  // namespace webserver
