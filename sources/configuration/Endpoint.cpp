#include "Endpoint.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>

#include "configuration/CgiHandlerConfig.hpp"
#include "configuration/RouteConfig.hpp"
#include "configuration/parser/ConfigParsingException.hpp"
#include "http_status/HttpStatus.hpp"
#include "logger/Logger.hpp"

using std::map;
using std::ostream;
using std::set;
using std::string;

namespace webserver {
const string Endpoint::DEFAULT_ROOT = "";
const string Endpoint::DEFAULT_INTERFACE = "127.0.0.1";
const int Endpoint::DEFAULT_PORT = 8888;

size_t Endpoint::defaultMaxClientBodySizeBytes() {
    // NOTE: cannot be a static constant field due to initialization order problems
    return (std::numeric_limits<std::streamsize>::max());
}

Endpoint::Endpoint()
    : _interface(DEFAULT_INTERFACE)
    , _port(DEFAULT_PORT)
    , _serverName("")
    , _rootDirectory(DEFAULT_ROOT)
    , _maxClientBodySizeBytes(defaultMaxClientBodySizeBytes())
    , _cgiHandlers()
    , _routes()
    , _statusCatalogue() {
}

Endpoint::Endpoint(const std::string& interface, int port)
    : _interface(interface)
    , _port(port)
    , _serverName("")
    , _rootDirectory(DEFAULT_ROOT)
    , _maxClientBodySizeBytes(defaultMaxClientBodySizeBytes())
    , _cgiHandlers()
    , _routes()
    , _statusCatalogue() {
}

Endpoint::Endpoint(const Endpoint& other)
    : _interface(other._interface)
    , _port(other._port)
    , _serverName(other._serverName)
    , _rootDirectory(other._rootDirectory)
    , _maxClientBodySizeBytes(other._maxClientBodySizeBytes)
    , _cgiHandlers()
    , _routes(other._routes)
    , _statusCatalogue(other._statusCatalogue) {
    for (map<std::string, CgiHandlerConfig*>::const_iterator it = other._cgiHandlers.begin();
         it != other._cgiHandlers.end();
         ++it) {
        _cgiHandlers[it->first] = (it->second == NULL ? NULL : new CgiHandlerConfig(*it->second));
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
        _cgiHandlers[it->first] = (it->second == NULL ? NULL : new CgiHandlerConfig(*it->second));
    }

    _interface = other._interface;
    _port = other._port;
    _serverName = other._serverName;
    _rootDirectory = other._rootDirectory;
    _maxClientBodySizeBytes = other._maxClientBodySizeBytes;
    _routes = other._routes;
    _statusCatalogue = other._statusCatalogue;

    return (*this);
}

int Endpoint::getPort(void) const {
    return (_port);
}

string Endpoint::getInterface(void) const {
    return (_interface);
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
    if (_statusCatalogue != other._statusCatalogue) {
        return (false);
    }

    return (true);
}

Endpoint::~Endpoint() {
    Logger log;
    log.stream(LOG_TRACE) << "Endpoint destroyed at " << this << "\n";
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
    if (size > defaultMaxClientBodySizeBytes()) {
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

Endpoint& Endpoint::addRoute(RouteConfig route) {
    route.setStatusCatalogue(_statusCatalogue);
    const std::pair<std::set<RouteConfig>::iterator, bool> result = _routes.insert(route);
    if (!result.second) {
        throw ConfigParsingException(
            "Duplicate location path '" + route.getPath() + "' in server block"
        );
    }
    return (*this);
}

const set<RouteConfig>& Endpoint::getRoutes() const {
    return (_routes);
}

string Endpoint::getRoot() const {
    return (_rootDirectory);
}

string Endpoint::getServerName() const {
    return (_serverName);
}

const RouteConfig& Endpoint::getRoute(std::string route) const {
    for (std::set<RouteConfig>::const_iterator it = _routes.begin(); it != _routes.end(); ++it) {
        if (it->getPath() == route) {
            return (*it);
        }
    }
    throw std::out_of_range("Route not found");
}

const RouteConfig& Endpoint::selectRoute(std::string route) const {
    set<RouteConfig>::const_iterator bestMatch = _routes.end();
    size_t bestLength = 0;
    Logger log;
    for (set<RouteConfig>::const_iterator itr = _routes.begin(); itr != _routes.end(); ++itr) {
        const string candidate = itr->getPath();
        if (candidate == "/" && bestLength == 0) {
            bestMatch = itr;
            bestLength = 1;
            continue;
        }
        log.stream(LOG_TRACE) << "matching " << candidate << "\n";
        if (route == candidate ||
            (route.substr(0, candidate.length()) == candidate &&
             route.length() > candidate.length() && route.at(candidate.length()) == '/')) {
            if (itr->getPath().length() > bestLength) {
                bestMatch = itr;
                bestLength = itr->getPath().length();
            }
        }
    }
    if (bestLength == 0) {
        throw std::out_of_range("Route not found; is there a root location in the configuration?");
    }
    return (*bestMatch);
}

const std::map<std::string, CgiHandlerConfig*>& Endpoint::getCgiHandlers() const {
    return (_cgiHandlers);
}

const HttpStatus& Endpoint::getStatusCatalogue() const {
    return (_statusCatalogue);
}

Endpoint& Endpoint::setStatusCatalogue(const HttpStatus& statusCatalogue) {
    _statusCatalogue = statusCatalogue;
    return (*this);
}

Endpoint& Endpoint::setStatusPage(int code, const std::string& pageFileLocation) {
    _statusCatalogue.setPage(code, pageFileLocation);
    return (*this);
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
    oss << "\n";
    for (map<string, CgiHandlerConfig*>::const_iterator itr = endpoint._cgiHandlers.begin();
         itr != endpoint._cgiHandlers.end();
         itr++) {
        oss << "{" << itr->first << ": " << *(itr->second) << "}\n";
    }
    oss << "\n";
    for (set<RouteConfig>::const_iterator itr = endpoint._routes.begin();
         itr != endpoint._routes.end();
         itr++) {
        oss << "{" << *itr << "}\n";
    }
    oss << "\nstatus catalogue:\n";
    oss << endpoint._statusCatalogue << "\n";
    return (oss);
}
}  // namespace webserver
