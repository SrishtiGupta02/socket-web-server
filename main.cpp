#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

const int PORT = 8080;
const int BUFFER_SIZE = 4096;

void send404(SOCKET clientSocket) {
    std::string content = "<html><body><h1>404 Not Found</h1><p>The requested file was not found on this server.</p></body></html>";
    std::ostringstream response;
    response << "HTTP/1.1 404 Not Found\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Content-Length: " << content.length() << "\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << content;

    send(clientSocket, response.str().c_str(), response.str().length(), 0);
}

void send403(SOCKET clientSocket) {
    std::string content = "<html><body><h1>403 Forbidden</h1><p>You do not have permission to access this resource.</p></body></html>";
    std::ostringstream response;
    response << "HTTP/1.1 403 Forbidden\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Content-Length: " << content.length() << "\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << content;

    send(clientSocket, response.str().c_str(), response.str().length(), 0);
}

void handleConnection(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE] = {0};

    int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
    if (bytesReceived <= 0) {
        closesocket(clientSocket);
        return;
    }

    std::string request(buffer);

    size_t method_end = request.find(' ');
    if (method_end == std::string::npos) {
        closesocket(clientSocket);
        return;
    }

    size_t path_end = request.find(' ', method_end + 1);
    if (path_end == std::string::npos) {
        closesocket(clientSocket);
        return;
    }

    std::string path = request.substr(method_end + 1, path_end - (method_end + 1));

    if (path == "/") {
        path = "/index.html";
    }

    if (path.find("..") != std::string::npos) {
        send403(clientSocket);
        closesocket(clientSocket);
        return;
    }

    std::string filePath = path.substr(1);

    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "File not found: " << filePath << std::endl;
        send404(clientSocket);
        closesocket(clientSocket);
        return;
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::ostringstream responseHeader;
    responseHeader << "HTTP/1.1 200 OK\r\n";
    responseHeader << "Content-Type: text/html\r\n";
    responseHeader << "Content-Length: " << fileSize << "\r\n";
    responseHeader << "Connection: close\r\n";
    responseHeader << "\r\n";

    send(clientSocket, responseHeader.str().c_str(), responseHeader.str().length(), 0);

    std::vector<char> fileBuffer(BUFFER_SIZE);
    while (file.read(fileBuffer.data(), fileBuffer.size())) {
        send(clientSocket, fileBuffer.data(), file.gcount(), 0);
    }
    send(clientSocket, fileBuffer.data(), file.gcount(), 0);

    std::cout << "Served file: " << filePath << std::endl;

    closesocket(clientSocket);
}

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaResult != 0) {
        std::cerr << "WSAStartup failed: " << wsaResult << std::endl;
        return 1;
    }
#endif

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(listenSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(listenSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    std::cout << "Server listening on http://localhost:" << PORT << std::endl;
    std::cout << "Press Ctrl+C to stop the server." << std::endl;

    while (true) {
        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        std::cout << "Client connected..." << std::endl;
        handleConnection(clientSocket);
    }

    closesocket(listenSocket);
#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
