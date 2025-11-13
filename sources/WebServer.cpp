#include "WebServer.hpp"

#include <csignal>
#include <cstddef>
#include <iostream>
#include <set>
#include <string>

#include "listener/MasterListener.hpp"

using std::cerr;
using std::endl;
using std::pair;
using std::set;
using std::string;

namespace webserver {
WebServer& WebServer::operator=(const WebServer& other) {
    if (&other == this) {
        return (*this);
    }
    cerr << "Unexpected stub assignment operator called for WebServer." << endl;
    _appConfig = other.getAppConfig();
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

AppConfig* WebServer::getAppConfig() const {
    return _appConfig;
}

WebServer::WebServer(const std::string& configFilePath)
    : _isRunning(0) {
    _appConfig = new AppConfig(configFilePath);
    handleSignals();
    // stub implementation, since AppConfig has a stub structure
    set<pair<string, int> > onePair;
    onePair.insert(pair<string, int>(_appConfig->getInterface(), _appConfig->getPort()));
    _masterListener = new MasterListener(onePair);
}

WebServer& WebServer::getInstance(const std::string& configFilePath) {
    static WebServer instance = WebServer(configFilePath);
    return instance;
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
