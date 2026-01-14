# Custom HTTP Web Server using C++ Sockets

## Overview
This project implements a lightweight HTTP web server from scratch using C++ socket programming.
The server listens for TCP connections, parses HTTP GET requests, and serves static HTML pages directly to a web browser without using external web servers like Apache or Nginx.

## Features
- Custom TCP server using C++ sockets
- Handles HTTP GET requests
- Serves multiple static HTML pages
- Implements proper HTTP response headers
- Handles 403 and 404 error responses
- Cross-platform socket structure (Windows/Linux)

## Tech Stack
- Language: C++
- Networking: TCP Sockets (Winsock / BSD)
- Protocol: HTTP/1.1
- Frontend: HTML, CSS

## Project Structure
custom-http-web-server-cpp/
- main.cpp
- index.html
- about.html
- code.html
- ai_tutor.html
- future_scope.html
- thankyou.html
- README.md

## How to Run
1. Compile the server:
   g++ main.cpp -o server -lws2_32

2. Run the executable:
   ./server

3. Open browser:
   http://localhost:8080

## Learning Outcomes
- Understanding of socket programming
- HTTP protocol fundamentals
- Client-server architecture
- File handling and request parsing

## Author
Srishti Gupta  
B.Tech Information Technology  
Jaypee Institute of Information Technology, Noida
