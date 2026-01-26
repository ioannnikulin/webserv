#include "ConfigParser.hpp"

#include <cctype>
#include <cerrno>
#include <cstring>
#include <string>

#include "configuration/AppConfig.hpp"
#include "configuration/Endpoint.hpp"
#include "configuration/parser/ConfigChecker.hpp"
#include "configuration/parser/ConfigParsingException.hpp"
#include "logger/Logger.hpp"

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
    if (_tokens.empty()) {
        throw ConfigParsingException("Configuration file is empty or contains only comments");
    }

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

    ConfigChecker::checkNoDuplicateEndpoints(appConfig.getEndpoints());
    return (appConfig);
}

void ConfigParser::parseServer(AppConfig& appConfig) {
    Logger log;
    Endpoint server;
    bool listenSet = false;
    bool bodySizeSet = false;

    if (_tokens[_index] != "{") {
        throw ConfigParsingException("Unexpected token: " + _tokens[_index]);
    }
    _index++;

    if (_tokens[_index] == "}") {
        throw ConfigParsingException("Empty block");
    }

    while (!isEnd(_tokens, _index)) {
        const string token = _tokens[_index];
        log.stream(LOG_TRACE) << "checking token " << token << "\n";

        if (token == "listen") {
            if (listenSet) {
                throw ConfigParsingException(
                    "Duplicate 'listen' directive (only one allowed per server block)"
                );
            }
            parseListen(server);
            listenSet = true;
        } else if (token == "server_name") {
            parseServerName(server);
        } else if (token == "root") {
            parseRoot(server);
        } else if (token == "location") {
            parseLocation(server);
        } else if (token == "client_max_body_size") {
            if (bodySizeSet) {
                throw ConfigParsingException(
                    "Duplicate 'client_max_body_size' directive (only one allowed per scope)"
                );
            }
            parseBodySize(server);
            bodySizeSet = true;
        } else if (token == "error_page") {
            parseErrorPage(server);
        } else if (token == "cgi") {
            parseCgi(server);
        } else if (token != "}") {
            throw ConfigParsingException("Unexpected token in server block: " + token);
        } else {
            if (!listenSet) {
                throw ConfigParsingException("Missing 'listen' directive in server block");
            }
            _index++;
            webserver::ConfigChecker::checkEndpoint(server);
            appConfig.addEndpoint(server);
            return;
        }
    }
    throw ConfigParsingException("Unexpected end of file in server block (missing '}')");
}

}  // namespace webserver
