#include "ConfigParser.hpp"

#include <cctype>
#include <cerrno>
#include <cstring>
#include <string>

#include "configuration/AppConfig.hpp"
#include "configuration/Endpoint.hpp"
#include "configuration/parser/ConfigChecker.hpp"
#include "configuration/parser/ConfigParsingException.hpp"

using std::string;

namespace webserver {

ConfigParser::ConfigParser()
    : _index(0) {
}
ConfigParser::~ConfigParser() {
}

AppConfig ConfigParser::parse(const string& filename) {
    tokenize(filename);
    return (buildConfigTree());
}

AppConfig ConfigParser::buildConfigTree() {
    AppConfig appConfig;

    while (!isEnd(_tokens, _index)) {
        const string token = _tokens[_index];

        if (token == "server") {
            _index++;
            parseServer(appConfig);
        } else {
            throw ConfigParsingException("Unexpected token: " + token);
        }
    }
    return (appConfig);
}

void ConfigParser::parseServer(AppConfig& appConfig) {
    Endpoint server;
    if (_tokens[_index] != "{") {
        throw ConfigParsingException("Unexpected token: " + _tokens[_index]);
    }
    _index++;

    while (!isEnd(_tokens, _index)) {
        const string token = _tokens[_index];

        if (token == "listen") {
            parseListen(server);
        } else if (token == "server_name") {
            parseServerName(server);
        } else if (token == "root") {
            parseRoot(server);
        } else if (token == "location") {
            parseLocation(server);
        } else if (token == "client_max_body_size") {
            parseBodySize(server);
        } else if (token == "error_page") {
            parseErrorPage();
        } else if (token == "cgi") {
            parseCgi(server);
        } else if (token != "}") {
            throw ConfigParsingException("Unexpected token in server block: " + token);
        } else {
            _index++;
            webserver::ConfigChecker::checkEndpoint(server);
            appConfig.addEndpoint(server);
            return;
        }
    }
    throw ConfigParsingException("Unexpected end of file in server block (missing '}')");
}

}  // namespace webserver
