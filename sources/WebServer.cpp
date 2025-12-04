#include "WebServer.hpp"

#include <csignal>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>

#include "configuration/AppConfig.hpp"
#include "http_status/HttpStatus.hpp"
#include "listener/MasterListener.hpp"
#include "utils/colors.hpp"
#include "utils/utils.hpp"

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
    utils::setColor(B_RED);
    std::cout << "\n❌ Webserver stopped after Ctrl+C." << std::endl;
    utils::resetColor();
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
    HttpStatus::initStatusMap();
    (void)configFilePath;
    _appConfig = new AppConfig();
    handleSignals();
    _masterListener = new MasterListener(_appConfig->getAllInterfacePortPairs());
}

WebServer& WebServer::getInstance(const std::string& configFilePath) {
    static WebServer instance = WebServer(configFilePath);
    return (instance);
}

WebServer::~WebServer() {
    delete _appConfig;
}

void WebServer::start() {
    _isRunning = 1;
    _masterListener->listenAndHandle(_isRunning, _appConfig);
}

void WebServer::stop() {
    _isRunning = 0;
}
}  // namespace webserver
