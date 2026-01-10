#include "Endpoint.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <string>

#include "configuration/CgiHandlerConfig.hpp"
#include "configuration/RouteConfig.hpp"
#include "configuration/UploadConfig.hpp"

using std::endl;
using std::map;
using std::ostream;
using std::set;
using std::string;

namespace webserver {
const string Endpoint::DEFAULT_ROOT;
const string Endpoint::DEFAULT_INTERFACE = "127.0.0.1";
const int Endpoint::DEFAULT_PORT = 8888;

size_t Endpoint::defaultMaxClientBodySizeBytes() {
    // NOTE: cannot be a static constant field due to initialization order problems
    return (std::numeric_limits<size_t>::max());
}

Endpoint::Endpoint()
    : _interface(DEFAULT_INTERFACE)
    , _port(DEFAULT_PORT)
    , _rootDirectory(DEFAULT_ROOT)
    , _maxClientBodySizeBytes(defaultMaxClientBodySizeBytes()) {
}

Endpoint::Endpoint(const std::string& interface, int port)
    : _interface(interface)
    , _port(port)
    , _rootDirectory(DEFAULT_ROOT)
    , _maxClientBodySizeBytes(defaultMaxClientBodySizeBytes()) {
}

Endpoint::Endpoint(const Endpoint& other)
    : _interface(other._interface)
    , _port(other._port)
    , _serverName(other._serverName)
    , _rootDirectory(other._rootDirectory)
    , _maxClientBodySizeBytes(other._maxClientBodySizeBytes)
    , _routes(other._routes) {
    for (map<std::string, CgiHandlerConfig*>::const_iterator it = other._cgiHandlers.begin();
         it != other._cgiHandlers.end();
         ++it) {
        _cgiHandlers[it->first] = new CgiHandlerConfig(*it->second);
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

    // NOTE: copying

    for (map<std::string, CgiHandlerConfig*>::const_iterator it = other._cgiHandlers.begin();
         it != other._cgiHandlers.end();
         ++it) {
        _cgiHandlers[it->first] = new CgiHandlerConfig(*it->second);
    }

    _interface = other._interface;
    _port = other._port;
    _serverName = other._serverName;
    _rootDirectory = other._rootDirectory;
    _maxClientBodySizeBytes = other._maxClientBodySizeBytes;
    _routes = other._routes;

    return (*this);
}

int Endpoint::getPort(void) const {
    return (_port);
}

string Endpoint::getInterface(void) const {
    return (_interface);
}

string Endpoint::getRoot() const {
    return (_rootDirectory);
}

size_t Endpoint::getMaxClientBodySizeBytes() const {
    return (_maxClientBodySizeBytes);
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
    if (_maxClientBodySizeBytes != other._maxClientBodySizeBytes) {
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

    return (true);
}

Endpoint::~Endpoint() {
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

Endpoint& Endpoint::setMaxClientBodySizeBytes(size_t size) {
    if (size > static_cast<size_t>(defaultMaxClientBodySizeBytes())) {
        _maxClientBodySizeBytes = defaultMaxClientBodySizeBytes();
    } else {
        _maxClientBodySizeBytes = size;
    }
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

bool Endpoint::isAValidPort(int port) {
    return (port >= MIN_PORT && port <= MAX_PORT);
}

ostream& operator<<(ostream& oss, const Endpoint& endpoint) {
    oss << endpoint._interface;
    oss << " " << endpoint._port;
    oss << " " << endpoint._serverName;
    oss << " " << endpoint._rootDirectory;
    oss << " " << endpoint._maxClientBodySizeBytes;
    oss << endl;
    for (map<string, CgiHandlerConfig*>::const_iterator itr = endpoint._cgiHandlers.begin();
         itr != endpoint._cgiHandlers.end();
         itr++) {
        oss << "{" << itr->first << ": " << *(itr->second) << "}" << endl;
    }
    oss << endl;
    for (set<RouteConfig>::const_iterator itr = endpoint._routes.begin();
         itr != endpoint._routes.end();
         itr++) {
        oss << "{" << *itr << "}" << endl;
    }
    oss << endl;
    return (oss);
}
}  // namespace webserver
