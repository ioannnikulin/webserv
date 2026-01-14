#pragma once
#ifndef LISTENER_HPP
#define LISTENER_HPP

#include <netinet/in.h>

#include <map>
#include <string>

#include "configuration/AppConfig.hpp"
#include "connection/Connection.hpp"
#include "logger/Logger.hpp"

namespace webserver {
class Listener {
private:
    Listener();
    Listener& operator=(const Listener& other);
    Listener(const Listener& other);

    static Logger _log;

    std::string _interface;
    int _port;
    int _listeningSocketFd;

    std::map<int, Connection*> _clientConnections;
    // NOTE: client socket file descriptor: connection
    const Endpoint& _configuration;

    struct ::sockaddr_in resolveAddress() const;

public:
    Listener(const Endpoint& configuration);

    /* NOTE: a Connection creates a socket file descriptor on itself,
    * then we pass it up to MasterListener so that it can create a proper pollfd,
    * push it into the queue, assign events.
    * The Connection keeps the numeric fd value for recv().
    */
    int getListeningSocketFd() const;
    bool hasActiveClientSocket(int clientSocketFd) const;

    int acceptConnection();  // NOTE: returns client socket fd
    Connection::State receiveRequest(int clientSocketFd);
    Connection::State generateResponse(int clientSocketFd);
    std::string getResponse(int clientSocketFd) const;
    Listener& setResponse(int clientSocketFd, std::string response);
    void sendResponse(int clientSocketFd);
    void killConnection(int clientSocketFd);

    ~Listener();
};
}  // namespace webserver
#endif
