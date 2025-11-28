#pragma once
#ifndef LISTENER_HPP
#define LISTENER_HPP

#include <netinet/in.h>

#include <map>
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
    std::map<int, Connection*> _clientConnections;  // NOTE: client socket: connection

    struct ::sockaddr_in resolveAddress() const;

public:
    Listener(const std::string& interface, int port);

    /* NOTE: a Connection creates a socket file descriptor on itself,
    * then we pass it up to MasterListener so that it can create a proper pollfd,
    * push it into the queue, assign events,
    * and transfer the pointer to that pollfd down to Connection again,
    * so that the Connection can change the events if needed
    */
    /* NOTE: maybe can simplify this?
    * MasterListener's queue cannot consist of references,
    * so the pollfd object will be created upstairs in MasterListener,
    * not in Connection alongside the file descriptor.
    * an obvious choice would be to create the file descriptor in MasterListener.
    * I'd prefer to create everything wherever relevant, so Connection's client socket in Connection,
    * but we don't have much choice here with poll.
    */
    void setClientSocket(::pollfd* clientSocket);
    int getListeningSocketFd() const;
    bool hasActiveClientSocket(int clientSocketFd) const;

    int acceptConnection();  // NOTE: returns client socket fd
    void receiveRequest(const ::pollfd& clientSocketFd);
    void sendResponse(int clientSocketFd);
    void killConnection(int clientSocketFd);

    ~Listener();
};
}  // namespace webserver
#endif
