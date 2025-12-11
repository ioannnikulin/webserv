#ifndef SHUTTINGDOWN_HPP
#define SHUTTINGDOWN_HPP

#include <exception>

namespace webserver {
class ShuttingDown : public std::exception {
private:
    ShuttingDown& operator=(const ShuttingDown& other);

public:
    ShuttingDown();
    ShuttingDown(const ShuttingDown& other);
    ~ShuttingDown() throw();
    const char* what() const throw();
};
}  // namespace webserver
#endif
