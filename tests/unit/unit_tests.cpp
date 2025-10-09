#include "tests.hpp"

int unit_tests() {
	std::string request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
	std::string expected_response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
	std::string actual_response = generate_response(request);

	assert(actual_response == expected_response);
	return (0);
}
