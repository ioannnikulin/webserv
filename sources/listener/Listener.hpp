#pragma once
#ifndef LISTENER_HPP
#define LISTENER_HPP
#include <string>

#include "Connection.hpp"

namespace webserver {
class Listener {
private:
    Listener();
    Listener& operator=(const Listener& other);
    Listener(const Listener& other);

    std::string _interface;
    int _port;
    int _listeningSocketFd;
    std::map<int, Connection*> _clientConnections;  // client socket: connection

public:
    Listener(const std::string& interface, int port);

    int getListeningSocketFd() const;
    bool hasActiveClientSocket(int clientSocketFd) const;

    int acceptConnection();  // returns client socket fd
    void receiveRequest(int clientSocketFd);

    ~Listener();
};
}  // namespace webserver
#endif