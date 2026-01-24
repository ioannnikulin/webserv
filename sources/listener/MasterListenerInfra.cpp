#include <cerrno>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "MasterListener.hpp"
#include "configuration/AppConfig.hpp"
#include "configuration/Endpoint.hpp"
#include "listener/Listener.hpp"
#include "logger/Logger.hpp"

using std::map;
using std::ostringstream;
using std::set;
using std::string;

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
