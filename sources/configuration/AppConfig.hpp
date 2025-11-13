#include <string>

namespace webserver {
class AppConfig {
private:
    std::string _interface;
    int _port;
    static const std::string _defaultInterface;
    static const int _defaultPort = 8888;
    AppConfig();
    AppConfig(const AppConfig& other);
    AppConfig& operator=(const AppConfig& other);

public:
    explicit AppConfig(std::string filePath);
    ~AppConfig();

    std::string getInterface() const;
    int getPort() const;
};
}  // namespace webserver

