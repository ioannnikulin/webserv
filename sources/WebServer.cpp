#include "WebServer.hpp"

#include <csignal>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>

#include "configuration/AppConfig.hpp"
#include "configuration/parser/ConfigParser.hpp"
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
    _appConfig = AppConfig(other.getAppConfig());
    _isRunning = 0;
    return (*this);
}

extern "C" void handleSigint(int signum) {
    (void)signum;
    WebServer& server = WebServer::getInstance("");
    server.stop();
    std::cout << B_RED << "\n❌ Webserver stopped after Ctrl+C." << RESET_COLOR << std::endl;
}

void WebServer::handleSignals() {
    signal(SIGINT, handleSigint);
}

AppConfig WebServer::getAppConfig() const {
    return (_appConfig);
}

WebServer::WebServer(const std::string& configFilePath)
    : _appConfig(ConfigParser().parse(configFilePath))
    , _isRunning(0)
    , _masterListener(_appConfig) {
    HttpStatus::initStatusMap();
    handleSignals();
}

WebServer& WebServer::getInstance(const string& configFilePath) {
    static WebServer instance = WebServer(configFilePath);
    return (instance);
}

WebServer::~WebServer() {
}

void WebServer::start() {
    _isRunning = 1;
    _masterListener.listenAndHandle(_isRunning);
}

void WebServer::stop() {
    _isRunning = 0;
}
}  // namespace webserver
