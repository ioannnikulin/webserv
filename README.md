*This project has been created as part of the 42 Core Curriculum by inikulin, lshapkin, and dmlasko*

# webserv

## Description

**webserv** is a HTTP/1.0 web server written in **C++** from scratch.  
The purpose of this project is to gain a deep understanding of how web servers work internally by reimplementing core features commonly found in production servers such as **NGINX**, without using any external web frameworks.

The project focuses on low-level system programming, networking, and protocol compliance. It emphasizes correctness, robustness, and performance while respecting the constraints of the HTTP/1.0 specification.

Through this project, we explore how clients and servers communicate, how requests are parsed and routed, and how responses are generated and sent efficiently.

---

## Features

- HTTP/1.0 request parsing and response handling
- Support for **GET**, **POST**, and **DELETE** methods
- Static file serving
- Multiple server blocks with different ports and hostnames
- Location-based routing
- Redirections
- Custom error pages
- CGI execution (Python, PHP scripts)
- Non-blocking I/O using a single event loop (`poll()`)
- Configuration file syntax inspired by NGINX

---

## Instructions

### Requirements

- Linux operating system
- A C++ compiler supporting **C++98**
- `make`

### Compilation
To compile, run:

```bash
make
```

### Launch
To run the server with a configuration file:

```bash
./webserv <config_file>
```

For local development, use:

```bash
make run
```
which runs this command:
```bash
./webserv ./tests/config_files/local_run.conf
```
for the demo website, run this:
```bash
./webserv ./tests/config_files/demo.conf
```

The server will listen on the address and ports defined in the configuration file.

---

## Resources
### Videos
#### Web Server Concepts and Examples
https://www.youtube.com/watch?v=9J1nJOivdyw

#### Building a Web Server in C++
https://www.youtube.com/watch?v=Kc1kwm1WyVM&list=PLPUbh_UILtZW1spXkwszDBN9YhtB0tFKC&index=1

### Readings
#### HTTP Server: Everything you need to know to Build a simple HTTP server from scratch
https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa$0

#### RFC 2616 (HTTP/1.1)
https://www.ietf.org/rfc/rfc2616.txt

#### RFC 1945 (HTTP/1.0)
https://www.rfc-editor.org/rfc/rfc1945.html