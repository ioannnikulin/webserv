#ifndef SHUTDOWNHANDLER_HPP
#define SHUTDOWNHANDLER_HPP

#include <exception>

namespace webserver {
class ShutdownHandler {
private:
    ShutdownHandler();
    ShutdownHandler(const ShutdownHandler& other);
    ShutdownHandler& operator=(const ShutdownHandler& other);
    ~ShutdownHandler();

public:
    static void handleRequest();
};
}  // namespace webserver
#endif
