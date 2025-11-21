#include "AppConfig.hpp"

#include <set>
#include <string>
#include <vector>

using std::pair;
using std::set;
using std::string;
using std::vector;

namespace webserver {

const int AppConfig::DEFAULT_MAX_BODY_SIZE = 1048576;

AppConfig::AppConfig()
    : _maxRequestBodySizeBytes(DEFAULT_MAX_BODY_SIZE) {
    _endpoints.push_back(Endpoint());
}

AppConfig::~AppConfig() {
}

set<pair<string, int> > AppConfig::getAllInterfacePortPairs(void) const {
    set<pair<string, int> > result;

    for (vector<Endpoint>::const_iterator it = _endpoints.begin(); it != _endpoints.end(); it++) {
        result.insert(pair<string, int>(it->getInterface(), it->getPort()));
    }
    return (result);
}

}  // namespace webserver
