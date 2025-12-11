#ifndef HTTPMETHODTYPE_HPP
#define HTTPMETHODTYPE_HPP

#include <map>
#include <string>

namespace webserver {
enum HttpMethodType { GET, POST, DELETE, SHUTDOWN };

HttpMethodType stringToMethod(const std::string& str);
}  // namespace webserver
#endif
