#include "WebServer.hpp"

#include <csignal>
#include <string>

#include "configuration/AppConfig.hpp"
#include "configuration/parser/ConfigParser.hpp"
#include "http_status/HttpStatus.hpp"
#include "listener/MasterListener.hpp"
#include "logger/Logger.hpp"

using std::string;

namespace webserver {

Logger WebServer::_log;

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
    // TODO 137: DL: Calling server.stop() from inside a signal handler is not signal-safe — signal handler should only set a flag.
    server.stop();
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
    handleSignals();
}

WebServer& WebServer::getInstance(const string& configFilePath) {
    HttpStatus::initStatusMap();
    static WebServer instance = WebServer(configFilePath);
    return (instance);
}

WebServer::~WebServer() {
}

void WebServer::start() {
    _isRunning = 1;
    _log.stream(LOG_INFO) << "Webserver started\n";
    _masterListener.listenAndHandle(_isRunning);
}

void WebServer::stop() {
    _isRunning = 0;
    _log.stream(LOG_INFO) << "Webserver stopped\n";
}
}  // namespace webserver
