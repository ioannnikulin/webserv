#pragma once
#ifndef MASTERLISTENER_HPP
#define MASTERLISTENER_HPP

#include <poll.h>

#include <map>
#include <set>
#include <vector>

#include "Listener.hpp"
#include "configuration/AppConfig.hpp"

namespace webserver {
class MasterListener {
private:
    MasterListener(const MasterListener& other);

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

    void handleIncomingConnection(::pollfd& activeFd, bool shouldDeny);
    void handleOutgoingConnection(const ::pollfd& activeFd);

public:
    MasterListener();
    explicit MasterListener(const AppConfig& configuration);
    MasterListener& operator=(const MasterListener& other);
    void listenAndHandle(volatile __sig_atomic_t& isRunning);
    ~MasterListener();
};
}  // namespace webserver
#endif
