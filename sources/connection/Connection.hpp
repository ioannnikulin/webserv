#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <poll.h>
#include <stdint.h>

#include <map>
#include <sstream>
#include <string>

#include "configuration/AppConfig.hpp"
#include "request/Request.hpp"

namespace webserver {
class Connection {
private:
    enum State { NEWBORN, READING, WRITING, CLOSED };
    State _state;
    ::pollfd* _clientSocket;  // NOTE: do not delete it here! it's managed by MasterListener
    int _clientSocketFd;
    std::string _responseBuffer;
    std::ostringstream _requestBuffer;
    Request* _request;
    uint32_t _clientIp;
    uint16_t _clientPort;

    Connection(const Connection& other);
    Connection& operator=(const Connection& other);

    bool fullRequestReceived();
    void receiveRequestContent();
    void markResponseReadyForReturn();

public:
    Connection();
    explicit Connection(int listeningSocketFd);
    ~Connection();

    int getClientSocketFd() const;
    Connection& setClientSocket(::pollfd* clientSocket);
    Connection& setResponseBuffer(std::string buffer);
    std::string getResponseBuffer() const;

    void handleRequest(const AppConfig* appConfig, bool shouldDeny);
    void sendResponse();
};
}  // namespace webserver
#endif
