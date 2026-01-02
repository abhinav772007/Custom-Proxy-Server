#define _WIN32_WINNT 0x0601
#include "../include/forwarder.h"
#include "../include/filter.h"   
#include <cstring>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
using namespace std;

SOCKET open_connection(const std::string &host, int port) {
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "[-] Socket creation failed\n";
        return INVALID_SOCKET;
    }
    struct addrinfo hints{}, *res = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    string port_str = to_string(port > 0 ? port : 80);

    if (getaddrinfo(host.c_str(), port_str.c_str(), &hints, &res) != 0) {
        cerr << "[-] DNS resolution failed\n";
        closesocket(server_socket);
        return INVALID_SOCKET;
    }

    if (connect(server_socket, res->ai_addr, (int)res->ai_addrlen) == SOCKET_ERROR) {
        cerr << "[-] Connection failed\n";
        freeaddrinfo(res);
        closesocket(server_socket);
        return INVALID_SOCKET;
    }

    freeaddrinfo(res);
    return server_socket;
}

bool forward_request(int client_socket, const HttpRequest &request) {

    if (isblocked(request.host)) {
        const char *resp =
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Length: 0\r\n"
            "Connection: close\r\n\r\n";
        send(client_socket, resp, (int)strlen(resp), 0);
        return false;
    }

    SOCKET server_socket = open_connection(request.host, request.port);
    if (server_socket == INVALID_SOCKET)
        return false;

    const char *data = request.raw.c_str();
    size_t total = request.raw.size();
    size_t sent = 0;

    while (sent < total) {
        int n = send(server_socket, data + sent, (int)(total - sent), 0);
        if (n == SOCKET_ERROR) {
            cerr << "[-] Failed to send request\n";
            closesocket(server_socket);
            return false;
        }
        sent += n;
    }

    char buffer[4096];
    int bytesRead;

    while ((bytesRead = recv(server_socket, buffer, sizeof(buffer), 0)) > 0) {
        int written = 0;
        while (written < bytesRead) {
            int n = send(client_socket, buffer + written, bytesRead - written, 0);
            if (n == SOCKET_ERROR) {
                cerr << "[-] Failed to send response\n";
                closesocket(server_socket);
                return false;
            }
            written += n;
        }
    }

    closesocket(server_socket);
    return true;
}