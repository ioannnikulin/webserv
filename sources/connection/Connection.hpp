#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <poll.h>
#include <stdint.h>

#include <map>
#include <sstream>
#include <string>

#include "configuration/AppConfig.hpp"
#include "logger/Logger.hpp"
#include "request/Request.hpp"

namespace webserver {
class Connection {
public:
    enum State {
        NEWBORN,
        READING,
        READING_COMPLETE,
        BAD_REQUEST_READ,
        METHOD_NOT_ALLOWED,
        REROUTING_BACK_TO_CGI,
        RECEIVED_RESPONSE_FROM_WORKER,
        RECEIVED_STATUS_FROM_WORKER,
        WRITING,
        WRITING_COMPLETE,
        RESPONSE_SENT,
        CLOSED_BY_CLIENT,
        SERVER_SHUTTING_DOWN,
        IGNORED
    };

private:
    static Logger _log;
    State _state;
    int _clientSocketFd;  // NOTE: acquired here, then passed to pollfd up in MasterListener
    std::string _responseBuffer;
    /* NOTE: buffer used for construction in a forked process,
    * then read in MasterListener via getResponseBuffer + responsePipe
    * and reset into the main thread's Connection for dispatching.
    */
    std::ostringstream _requestBuffer;
    Request _request;
    bool _isRequestValid;
    uint32_t _clientIp;
    uint16_t _clientPort;
    const Endpoint& _configuration;
    const RouteConfig* _route;

    Connection();
    Connection(const Connection& other);
    Connection& operator=(const Connection& other);

    bool fullRequestReceived();
    bool itsACgiRequest();
    std::string resolveScriptPath();
    static void cgiError(const char* errorMsg);

public:
    explicit Connection(int listeningSocketFd, const Endpoint& configuration);
    ~Connection();

    int getClientSocketFd() const;
    Connection& setResponseBuffer(std::string buffer);
    std::string getResponseBuffer() const;

    State receiveRequestContent();
    State generateResponse();
    void sendResponse();

    const CgiHandlerConfig* resolveCgiHandler(const Endpoint& config);
    Connection::State executeCgi(const Endpoint& config);
    std::string getRequestBody();
    const Request& getRequest() const;
};
}  // namespace webserver
#endif
