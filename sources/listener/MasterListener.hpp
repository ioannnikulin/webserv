#pragma once
#ifndef MASTER_LISTENER_HPP
#define MASTER_LISTENER_HPP

#include <poll.h>

#include <map>
#include <set>
#include <vector>

#include "Listener.hpp"
#include "configuration/AppConfig.hpp"

namespace webserver {
class MasterListener {
private:
    MasterListener();
    MasterListener(const MasterListener& other);
    MasterListener& operator=(const MasterListener& other);

    std::vector<struct ::pollfd> _pollFds;
    /* NOTE:
	* every Listener is created for a specific interface:port pair,
	* gets a LISTENING socket file descriptor.
	* when a new connection comes in on that socket,
	* we accept it and issue a CLIENT socket file descriptor for the same Listener.
	* both file descriptors are stored in Listener,
	* and also in these maps for easier navigation
	*/
    std::map<int, Listener*> _listeners;        // NOTE: listening socket fd: Listener
    std::map<int, Listener*> _clientListeners;  // NOTE: client socket fd: Listener

    void handleIncomingConnection(::pollfd& activeFd, const AppConfig* appConfig, bool shouldDeny);
    void handleOutgoingConnection(const ::pollfd& activeFd) const;

public:
    explicit MasterListener(const std::set<std::pair<std::string, int> >& interfacePortPairs);
    void listenAndHandle(volatile __sig_atomic_t& isRunning, const AppConfig* appConfig);
    ~MasterListener();
};
}  // namespace webserver
#endif
