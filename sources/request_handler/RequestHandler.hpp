#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP
#include <string>
namespace webserver {
class RequestHandler {
private:
    RequestHandler();
    RequestHandler(const RequestHandler& other);
    RequestHandler& operator=(const RequestHandler& other);

public:
    static std::string handle(std::string type, std::string location);
    ~RequestHandler();
};
}  // namespace webserver
#endif
