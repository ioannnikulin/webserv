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
public:
    enum State { NEWBORN, READING, WRITING, RESPONSE_READY, CLOSED, TERMINATE };

private:
    State _state;
    int _clientSocketFd; // NOTE: acquired here, then passed to pollfd up in MasterListener
    std::string _responseBuffer;
    std::ostringstream _requestBuffer;
    Request _request;
    uint32_t _clientIp;
    uint16_t _clientPort;
    const Endpoint& _configuration;

    Connection();
    Connection(const Connection& other);
    Connection& operator=(const Connection& other);

    bool fullRequestReceived();
    void receiveRequestContent();
    void markResponseReadyForReturn();

public:

	class TerminatedByClient: public std::exception {
    private:
        TerminatedByClient& operator=(const TerminatedByClient& other);

    public:
        TerminatedByClient();
        TerminatedByClient(const TerminatedByClient& other);
        ~TerminatedByClient() throw();
        const char* what() const throw();
    };
    explicit Connection(int listeningSocketFd, const Endpoint& configuration);
    ~Connection();

    int getClientSocketFd() const;
    Connection& setResponseBuffer(std::string buffer);
    std::string getResponseBuffer() const;

    State handleRequest(bool shouldDeny);
    void sendResponse();
};
}  // namespace webserver
#endif
