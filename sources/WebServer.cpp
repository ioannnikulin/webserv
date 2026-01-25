#include "WebServer.hpp"

#include <signal.h>

#include <csignal>
#include <string>

#include "configuration/AppConfig.hpp"
#include "configuration/parser/ConfigParser.hpp"
#include "listener/MasterListener.hpp"
#include "logger/Logger.hpp"
#include "signals/ServerSignal.hpp"

using std::string;

namespace webserver {

volatile sig_atomic_t WebServer::serverSignals = SIG_NONE;
Logger WebServer::_log;

WebServer& WebServer::operator=(const WebServer& other) {
    if (&other == this) {
        return (*this);
    }
    _appConfig = AppConfig(other.getAppConfig());
    _isRunning = 0;
    return (*this);
}

extern "C" void handleSigint(int signum) {  // NOTE: Ctrl+C
    (void)signum;
    WebServer::serverSignals |= SIG_SHUTDOWN;
}

extern "C" void handleSigterm(int signum) {  // NOTE: SIGTERM (kill, system stop)
    (void)signum;
    WebServer::serverSignals |= SIG_SHUTDOWN;
}

extern "C" void handleSigstp(int signum) {  // NOTE: Ctrl+Z
    (void)signum;
    WebServer::serverSignals |= SIG_SHUTDOWN;
}

void WebServer::handleSignals() {
    signal(SIGINT, handleSigint);
    signal(SIGTERM, handleSigterm);
    signal(SIGTSTP, handleSigstp);
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
    static WebServer instance = WebServer(configFilePath);
    return (instance);
}

WebServer::~WebServer() {
}

void WebServer::start() {
    _isRunning = 1;
    _log.stream(LOG_INFO) << "Webserver starting\n";
    _masterListener.listenAndHandle(_isRunning, serverSignals);
    _log.stream(LOG_INFO) << "Webserver stopped\n";
}
}  // namespace webserver
