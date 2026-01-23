#ifndef MASTERLISTENER_HPP
#define MASTERLISTENER_HPP

#include <poll.h>
#include <time.h>

#include <map>
#include <set>
#include <vector>

#include "Listener.hpp"
#include "cgi_handler/CgiProcessManager.hpp"
#include "configuration/AppConfig.hpp"

namespace webserver {
class MasterListener {
private:
    static Logger _log;

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
    std::map<int, int> _responseWorkerControls;
    // NOTE: reading pipe end fd with an expected control message: client socket fd
    std::map<int, int> _responseWorkers;
    // NOTE: reading pipe end fd with an expected generated response: client socket fd
    CgiProcessManager _cgiManager;

    MasterListener(const MasterListener& other);

    int registerNewConnection(int listeningFd, Listener* listener);
    void removePollFd(int fdesc);
    void populateFdsFromListeners();
    void
    registerResponseWorker(int controlPipeReadingEnd, int responsePipeReadingEnd, int clientFd);
    void markResponseReadyForReturn(int clientFd);
    Connection::State callCgi(Listener* listener, int activeFd);
    Connection::State generateResponse(Listener* listener, int activeFd);
    Connection::State isItANewConnectionOnAListeningSocket(int activeFd);
    Connection::State isItADataRequestOnAClientSocketFromARegisteredClient(::pollfd& activeFd);
    Connection::State isItAControlMessageFromAResponseGeneratorWorker(int activeFd);
    Connection::State isItAResponseFromAResponseGeneratorWorker(int activeFd);
    Connection::State handleIncomingConnection(::pollfd& activeFd, bool& acceptingNewConnections);
    void handleOutgoingConnection(const ::pollfd& activeFd);
    void resetPollEvents();
    void handlePollEvents(bool& acceptingNewConnections);
    static void reapChildren();
    void cleanupCgiProcess(int clientFd, bool sendTimeoutResponse);
    void checkCgiTimeouts();

public:
    MasterListener();
    explicit MasterListener(const AppConfig& configuration);
    MasterListener& operator=(const MasterListener& other);
    ~MasterListener();

    void listenAndHandle(volatile __sig_atomic_t& isRunning, volatile __sig_atomic_t& signals);
};
}  // namespace webserver
#endif
