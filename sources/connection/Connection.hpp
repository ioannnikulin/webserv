#include <poll.h>

#include <map>
#include <string>

#include "Request.hpp"

namespace webserver {
class Connection {
private:
    Connection();
    Connection(const Connection& other);
    Connection& operator=(const Connection& other);

    ::pollfd* _clientSocket;  // NOTE: do not delete it here! it's managed by MasterListener
    int _clientSocketFd;
    std::string _responseBuffer;
    Request* _request;

    std::string receiveRequestContent();
    void markResponseReadyForReturn();

public:
    explicit Connection(int listeningSocketFd);
    ~Connection();

    int getClientSocketFd() const;
    void setClientSocket(::pollfd* clientSocket);

    void handleRequest();
    void generateReponseHeaders();
    void sendResponse();
};
}  // namespace webserver
