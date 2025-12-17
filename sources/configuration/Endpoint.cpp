#include "Endpoint.hpp"

#include <cstddef>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "configuration/CgiHandlerConfig.hpp"
#include "configuration/RouteConfig.hpp"
#include "configuration/UploadConfig.hpp"

using std::string;
using std::vector;

namespace webserver {
const string Endpoint::DEFAULT_ROOT;
const string Endpoint::DEFAULT_INTERFACE = "127.0.0.1";
const int Endpoint::DEFAULT_PORT = 8888;
const unsigned long Endpoint::DEFAULT_MAX_BODY_SIZE = 1 << 20;

Endpoint::Endpoint()
    : _interface(DEFAULT_INTERFACE)
    , _port(DEFAULT_PORT)
    , _rootDirectory(DEFAULT_ROOT)
    , _maxRequestBodySizeBytes(DEFAULT_MAX_BODY_SIZE)
    , _uploadConfig(NULL) {
}

Endpoint::Endpoint(const std::string& interface, int port)
    : _interface(interface)
    , _port(port)
    , _rootDirectory(DEFAULT_ROOT)
    , _maxRequestBodySizeBytes(DEFAULT_MAX_BODY_SIZE)
    , _uploadConfig(NULL) {
}

Endpoint::Endpoint(const Endpoint& other)
    : _interface(other._interface)
    , _port(other._port)
    , _serverName(other._serverName)
    , _rootDirectory(other._rootDirectory)
    , _maxRequestBodySizeBytes(other._maxRequestBodySizeBytes)
    , _cgiHandlers(other._cgiHandlers)
    , _routes(other._routes)
    , _uploadConfig(NULL) {
    for (std::map<std::string, CgiHandlerConfig*>::const_iterator it = other._cgiHandlers.begin();
         it != other._cgiHandlers.end();
         ++it) {
        _cgiHandlers[it->first] = new CgiHandlerConfig(*(it->second));
    }
    if (other._uploadConfig != NULL) {
        _uploadConfig = new UploadConfig(*other._uploadConfig);
    }
}

Endpoint& Endpoint::operator=(const Endpoint& other) {
    if (this != &other) {
        _interface = other._interface;
        _port = other._port;
        _serverName = other._serverName;
        _rootDirectory = other._rootDirectory;
        _maxRequestBodySizeBytes = other._maxRequestBodySizeBytes;
        _cgiHandlers = other._cgiHandlers;
        _routes = other._routes;

        for (std::map<std::string, CgiHandlerConfig*>::iterator it = _cgiHandlers.begin();
             it != _cgiHandlers.end();
             ++it) {
            delete it->second;
        }
        _cgiHandlers.clear();

        for (std::map<std::string, CgiHandlerConfig*>::const_iterator it =
                 other._cgiHandlers.begin();
             it != other._cgiHandlers.end();
             ++it) {
            _cgiHandlers[it->first] = new CgiHandlerConfig(*(it->second));
        }

        delete _uploadConfig;

        _uploadConfig =
            (other._uploadConfig != NULL) ? new UploadConfig(*other._uploadConfig) : NULL;
    }
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

    std::map<std::string, CgiHandlerConfig*>::const_iterator it1 = _cgiHandlers.begin();
    std::map<std::string, CgiHandlerConfig*>::const_iterator it2 = other._cgiHandlers.begin();

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
    for (std::map<std::string, CgiHandlerConfig*>::iterator it = _cgiHandlers.begin();
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

Endpoint& Endpoint::setClientMaxBodySize(size_t size) {
    _maxRequestBodySizeBytes = size;
    return (*this);
}

Endpoint& Endpoint::addCgiHandler(const CgiHandlerConfig& config, string extension) {
    _cgiHandlers[extension] = new CgiHandlerConfig(config);
    return (*this);
}

Endpoint& Endpoint::addRoute(const RouteConfig& route) {
    _routes.push_back(route);
    return (*this);
}

vector<RouteConfig> Endpoint::getRoutes() const {
    return (_routes);
}

RouteConfig Endpoint::getRoute(std::string route) const {
    for (size_t i = 0; i < _routes.size(); ++i) {
        if (_routes[i].getPath() == route) {
            return (_routes[i]);
        }
    }
    throw std::runtime_error("Route not found");
}

Endpoint& Endpoint::setUploadConfig(const UploadConfig& cfg) {
    delete _uploadConfig;
    _uploadConfig = new UploadConfig(cfg);
    return (*this);
}

bool Endpoint::isAValidPort(int port) {
    return (port >= MIN_PORT && port <= MAX_PORT);
}
}  // namespace webserver
