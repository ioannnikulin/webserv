#include "CgiHandler.hpp"

#include <cstring>
#include <map>
#include <sstream>
#include <string>

#include "configuration/CgiHandlerConfig.hpp"
#include "configuration/RouteConfig.hpp"
#include "http_methods/HttpMethodType.hpp"
#include "request/Request.hpp"
#include "response/Response.hpp"

using std::string;

namespace webserver {

CgiHandler::CgiHandler(
    const CgiHandlerConfig& config,
    const Request& request,
    const string& scriptPath,
    int serverPort,
    const RouteConfig& route
)
    : _config(config)
    , _request(request)
    , _scriptPath(scriptPath)
    , _serverPort(serverPort)
    , _route(route) {
}

CgiHandler::~CgiHandler() {
}

string CgiHandler::resolveIndexPath() {
    string resolvedPath = _request.getPath();
    if (!resolvedPath.empty() && resolvedPath[resolvedPath.length() - 1] == '/') {
        const string indexFile = _route.getFolderConfig().getIndexPageFilename();
        if (!indexFile.empty()) {
            resolvedPath += indexFile;
        }
    }
    return (resolvedPath);
}

string CgiHandler::getMethod() {
    const HttpMethodType httpMethod = _request.getType();
    const string method = methodToString(httpMethod);
    return (method);
}

void CgiHandler::setupEnvironment() {
    addEnvVar("REQUEST_METHOD", getMethod());
    addEnvVar("SCRIPT_NAME", resolveIndexPath());
    addEnvVar("SCRIPT_FILENAME", _scriptPath);
    addEnvVar("QUERY_STRING", _request.getQuery());
    addEnvVar("PATH_INFO", resolveIndexPath());
    addEnvVar("REDIRECT_STATUS", "200");
    if (getMethod() == "POST") {
        const string contentType = _request.getHeader("Content-Type");
        if (!contentType.empty()) {
            addEnvVar("CONTENT_TYPE", contentType);
        }
        const string transferEncoding = _request.getHeader("Transfer-Encoding");
        if (transferEncoding == "chunked") {
            const string body = getRequestBody();
            std::ostringstream lengthStream;
            lengthStream << body.length();
            addEnvVar("CONTENT_LENGTH", lengthStream.str());
        } else {
            const string contentLength = _request.getHeader("Content-Length");
            if (!contentLength.empty()) {
                addEnvVar("CONTENT_LENGTH", contentLength);
            }
        }
    }
    addEnvVar("SERVER_SOFTWARE", SERVER_NAME);
    addEnvVar("SERVER_PROTOCOL", HTTP_PROTOCOL);
    addEnvVar("GATEWAY_INTERFACE", "CGI/1.1");

    const string host = _request.getHeader("Host");
    if (!host.empty()) {
        const size_t colonPos = host.find(':');
        if (colonPos != string::npos) {
            addEnvVar("SERVER_NAME", host.substr(0, colonPos));
        } else {
            addEnvVar("SERVER_NAME", host);
        }
    }
    std::ostringstream portStream;
    portStream << _serverPort;
    addEnvVar("SERVER_PORT", portStream.str());
}

void CgiHandler::addEnvVar(const string& key, const string& value) {
    _env[key] = value;
}

char** CgiHandler::getEnvArray() const {
    char** env = new char*[_env.size() + 1];
    size_t index = 0;

    for (std::map<string, string>::const_iterator it = _env.begin(); it != _env.end(); ++it) {
        string envStr = it->first + "=" + it->second;
        env[index] = new char[envStr.length() + 1];
        for (size_t j = 0; j < envStr.length(); ++j) {
            env[index][j] = envStr[j];
        }
        env[index][envStr.length()] = '\0';
        ++index;
    }
    env[index] = NULL;
    return (env);
}

void CgiHandler::freeEnvArray(char** env) {
    for (size_t i = 0; env[i] != NULL; ++i) {
        delete[] env[i];
    }
    delete[] env;
}

char** CgiHandler::prepareEnvironment() {
    setupEnvironment();
    return (getEnvArray());
}

std::string CgiHandler::getExecutablePath() const {
    return (_config.getExecutablePath());
}

std::string CgiHandler::getScriptPath() const {
    return (_scriptPath);
}

std::string CgiHandler::getRequestBody() {
    return (const_cast<Request&>(_request).getBody());
}
}  // namespace webserver
