#include "MasterListener.hpp"

#include <signal.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <cerrno>
#include <exception>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "configuration/AppConfig.hpp"
#include "configuration/Endpoint.hpp"
#include "connection/Connection.hpp"
#include "file_system/FileSystem.hpp"
#include "http_status/HttpStatus.hpp"
#include "listener/Listener.hpp"
#include "logger/Logger.hpp"
#include "response/Response.hpp"
#include "signals/ServerSignal.hpp"

using std::map;
using std::ostringstream;
using std::runtime_error;
using std::set;
using std::string;
using std::vector;

namespace webserver {

Logger MasterListener::_log;

MasterListener::MasterListener(const AppConfig& configuration)
    : _cgiManager(_log) {
    const set<Endpoint>& endpoints = configuration.getEndpoints();
    for (set<Endpoint>::const_iterator itr = endpoints.begin(); itr != endpoints.end(); ++itr) {
        Listener* newListener = new Listener(*itr);
        _listeners[newListener->getListeningSocketFd()] = newListener;
    }
}

MasterListener& MasterListener::operator=(const MasterListener& other) {
    if (this == &other) {
        return (*this);
    }
    _pollFds = other._pollFds;
    _listeners = other._listeners;
    _clientListeners = other._clientListeners;
    return (*this);
}

MasterListener::~MasterListener() {
    for (map<int, Listener*>::iterator it = _clientListeners.begin(); it != _clientListeners.end();
         ++it) {
        it->second->killConnection(it->first);
    }
    for (map<int, Listener*>::const_iterator it = _listeners.begin(); it != _listeners.end();
         ++it) {
        delete it->second;
    }  // NOTE: deleting from listeners only, clientListeners contains pointers to the same Listener objects
}

}  // namespace webserver
