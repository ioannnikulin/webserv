#include <poll.h>

#include <map>
#include <string>

#include "configuration/AppConfig.hpp"
#include "request/Request.hpp"

namespace webserver {
class Connection {
private:
    Connection(const Connection& other);
    Connection& operator=(const Connection& other);

    ::pollfd* _clientSocket;  // NOTE: do not delete it here! it's managed by MasterListener
    int _clientSocketFd;
    std::string _responseBuffer;
    Request* _request;

    std::string receiveRequestContent();
    void markResponseReadyForReturn();

public:
    Connection();
    explicit Connection(int listeningSocketFd);
    ~Connection();

    int getClientSocketFd() const;
    Connection& setClientSocket(::pollfd* clientSocket);
    Connection& setResponseBuffer(std::string buffer);
    std::string getResponseBuffer() const;

    void handleRequest(const AppConfig* appConfig);
    void sendResponse();
};
}  // namespace webserver
