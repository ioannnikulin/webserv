#include <string>

#include "RequestHandler.hpp"

namespace webserver {
class GetHandler {
private:
    GetHandler();
    GetHandler(const GetHandler& other);
    GetHandler& operator=(const GetHandler& other);

public:
    static std::string handle(std::string location);
    ~GetHandler();
};
}  // namespace webserver
