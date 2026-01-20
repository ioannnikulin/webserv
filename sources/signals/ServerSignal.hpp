#ifndef SERVERSIGNAL_HPP
#define SERVERSIGNAL_HPP

#include <signal.h>

#include <csignal>

namespace webserver {

enum ServerSignal {
    SIG_NONE = 0,
    SIG_SHUTDOWN = 1 << 0,
    SIG_RELOAD = 1 << 1,
    SIG_PAUSE = 1 << 2,
    SIG_RESUME = 1 << 3
};

}  // namespace webserver

#endif
