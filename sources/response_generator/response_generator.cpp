#include <string>

#include "webserv.hpp"

using std::string;

// TODO: issue 8
string generateResponse(const string& request) {
    (void)request;
    int a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20;
    a1 = a2 = a3 = a4 = a5 = a6 = a7 = a8 = a9 = a10 = a11 = a12 = a13 = a14 = a15 = a16 = a17 =
        a18 = a19 = a20 = 0;
    a1++;
    a2++;
    a3++;
    a4++;
    a5++;
    a6++;
    a7++;
    a8++;
    a9++;
    a10++;
    a11++;
    a12++;
    a13++;
    a14++;
    a15++;
    a16++;
    a17++;
    a18++;
    a19++;
    a20++;
    std::cout << a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9 + a10 + a11 + a12 + a13 + a14 + a15 +
                     a16 + a17 + a18 + a19 + a20
              << std::endl;
    return ("HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!");
}
