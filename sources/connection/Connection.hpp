#include <map>

using std::map;

namespace webserver {
class Connection {
private:
    Connection();
    Connection(const Connection& other);
    Connection& operator=(const Connection& other);

    int _clientSocketFd;

    string receiveRequestContent();
    void sendResponse(std::string response) const;

public:
    Connection(int listeningSocketFd);
    ~Connection();

    int getClientSocketFd() const;
    void handleRequest();
};
}  // namespace webserver