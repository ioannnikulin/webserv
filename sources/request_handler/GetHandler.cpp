#include "GetHandler.hpp"

#include <string>

using std::string;

namespace webserver {
string GetHandler::handle(string location) {
    /* TODO 58: a request handler must get more information as arguments. 
    at least the root folder. or maybe full path resolution 
    should happen on earlier stages?
    */
    (void)location;
    return ("Hello, World!");
}
}  // namespace webserver
