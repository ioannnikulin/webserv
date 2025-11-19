#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <string>

#include "configuration/AppConfig.hpp"

namespace webserver {
class WebServer {
private:
    WebServer();
    WebServer(const WebServer& other);
    WebServer& operator=(const WebServer& other);

    AppConfig* _appConfig;

public:
    explicit WebServer(const std::string& configFilePath);
    ~WebServer();

    void start();
    void stop();
};

std::string generateResponse(const std::string& request);
}  // namespace webserver
#endif
