#include "Endpoint.hpp"

#include <cstddef>
#include <string>
#include <vector>

#include "CgiHandlerConfig.hpp"
#include "RouteConfig.hpp"
#include "UploadConfig.hpp"

using std::string;

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
    if (_cgiHandlers != other._cgiHandlers) {
        return (false);
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
}

Endpoint& Endpoint::setInterface(string interface) {
    _interface = interface;
    return (*this);
}

void Endpoint::setPort(const int& port) {
    _port = port;
}

Endpoint& Endpoint::addServerName(const string& name) {
    _serverName = name;
    return (*this);
}

void Endpoint::setRoot(const string& path) {
    _rootDirectory = path;
}

void Endpoint::setClientMaxBodySize(size_t size) {
    _maxRequestBodySizeBytes = size;
}

void Endpoint::addCgiHandler(const CgiHandlerConfig& config, string extension) {
    _cgiHandlers[extension] = config;
}

void Endpoint::addRoute(const RouteConfig& route) {
    _routes.push_back(route);
}

std::vector<RouteConfig> Endpoint::getRoutes() const {
    return (_routes);
}

void Endpoint::setUploadConfig(const UploadConfig& cfg) {
    delete _uploadConfig;
    _uploadConfig = new UploadConfig(cfg);
}

bool Endpoint::isAValidPort(int port) {
    return (port > MIN_PORT && port <= MAX_PORT);
}
}  // namespace webserver
