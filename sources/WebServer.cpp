#include "WebServer.hpp"

#include <csignal>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>

#include "configuration/AppConfig.hpp"
#include "configuration/parser/ConfigParser.hpp"
#include "http_errors/HttpError.hpp"
#include "listener/MasterListener.hpp"

using std::cerr;
using std::endl;
using std::string;

namespace webserver {

WebServer& WebServer::operator=(const WebServer& other) {
    if (&other == this) {
        return (*this);
    }
    cerr << "Unexpected stub assignment operator called for WebServer." << endl;
    _appConfig = new AppConfig(other.getAppConfig());
    _isRunning = 0;
    _masterListener = NULL;
    return (*this);
}

extern "C" void handleSigint(int signum) {
    (void)signum;
    WebServer& server = WebServer::getInstance("");
    server.stop();
}

void WebServer::handleSignals() {
    signal(SIGINT, handleSigint);
}

AppConfig WebServer::getAppConfig() const {
    if (_appConfig == NULL) {
        throw std::runtime_error("Configuration uninitialized");
    }
    return (*_appConfig);
}

WebServer::WebServer(const std::string& configFilePath)
    : _isRunning(0) {
    HttpError::initializeErrors();
    ConfigParser parser;
    _appConfig = new AppConfig(parser.parse(configFilePath));
    handleSignals();
    _masterListener = new MasterListener(_appConfig->getAllInterfacePortPairs());
}

WebServer& WebServer::getInstance(const string& configFilePath) {
    static WebServer instance = WebServer(configFilePath);
    return (instance);
}

WebServer::~WebServer() {
    delete _appConfig;
}

void WebServer::start() {
    _isRunning = 1;
    _masterListener->listenAndHandle(_isRunning);
}

void WebServer::stop() {
    _isRunning = 0;
}
}  // namespace webserver
