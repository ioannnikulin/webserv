#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "configuration/CgiHandlerConfig.hpp"
#include "configuration/RouteConfig.hpp"
#include "request/Request.hpp"
#include "response/Response.hpp"

namespace webserver {

class CgiHandler {
private:
    const CgiHandlerConfig& _config;
    const Request& _request;
    const std::string& _scriptPath;
    int _serverPort;
    const RouteConfig& _route;
    std::map<std::string, std::string> _env;

    CgiHandler();
    CgiHandler(const CgiHandler& other);
    CgiHandler& operator=(const CgiHandler& other);

    void setupEnvironment();
    void addEnvVar(const std::string& key, const std::string& value);
    char** getEnvArray() const;
    static void freeEnvArray(char** env);

public:
    CgiHandler(
        const CgiHandlerConfig& config,
        const Request& request,
        const std::string& scriptPath,
        int serverPort,
        const RouteConfig& route
    );
    ~CgiHandler();

    char** prepareEnvironment();
    std::string getExecutablePath() const;
    std::string getScriptPath() const;
    std::string getRequestBody();
};
}  // namespace webserver

#endif
