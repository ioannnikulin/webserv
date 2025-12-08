#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <string>
#include <vector>

#include "configuration/AppConfig.hpp"
#include "configuration/parser/LocationTempData.hpp"

namespace webserver {
class ConfigParser {
private:
    LocationTempData _tmp;

    std::vector<std::string> _tokens;
    size_t _index;

    void tokenize(const std::string& filename);
    AppConfig buildConfigTree();
    void parseServer(AppConfig& appConfig);

    void parseListen(Endpoint& server);
    void parseServerName(Endpoint& server);
    void parseRoot(Endpoint& server);
    void parseBodySize(Endpoint& server);
    void parseErrorPage();
    void parseCgi(Endpoint& server);
    void parseLocation(Endpoint& server);
    void parseUpload(Endpoint& server);

    void parseLocationRoot();
    void parseLocationAutoindex();
    void parseLocationIndex();
    void parseLocationMethods(RouteConfig& route);
    void parseLocationLimitExcept(RouteConfig& route);
    void parseLocationReturn(RouteConfig& route, const std::string& locationPath);
    void parseLocationUpload();
    void parseLocationCgi(RouteConfig& route);

    static bool isEnd(const std::vector<std::string>& tokens, size_t index);

public:
    ConfigParser();
    ConfigParser(const ConfigParser& other);
    ConfigParser& operator=(const ConfigParser& other);
    ~ConfigParser();

    static const int BUFFER_SIZE = 4096;
    static const int KIB = 1024;
    static const int MIB = 1024 * 1024;
    static const int GIB = 1024 * 1024 * 1024;

    AppConfig parse(const std::string& filename);
};
}  // namespace webserver

#endif
