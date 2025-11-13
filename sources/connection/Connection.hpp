#include <poll.h>

#include <map>
#include <string>
using std::map;

namespace webserver {
class Connection {
private:
    Connection();
    Connection(const Connection& other);
    Connection& operator=(const Connection& other);

    ::pollfd* _clientSocket;  // NOTE: do not delete it here! it's managed by MasterListener
    int _clientSocketFd;
    std::string _responseBuffer;

    std::string receiveRequestContent();
    void markResponseReadyForReturn();

public:
    Connection(int listeningSocketFd);
    ~Connection();

    int getClientSocketFd() const;
    void setClientSocket(::pollfd* clientSocket);
    void handleRequest();
    void sendResponse();
};
}  // namespace webserver
