#define _WIN32_WINNT 0x0601
#include "../include/forwarder.h"
#include "../include/filter.h"   
#include <cstring>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
using namespace std;

static void set_timeouts(SOCKET sock){
    DWORD timeout=10000;
        setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(timeout));
        setsockopt(sock,SOL_SOCKET,SO_SNDTIMEO,(char*)&timeout,sizeof(timeout));
}
static void tunnel(SOCKET a,SOCKET b){
    char buffer[4096];
    fd_set readfds;
    while(true){
       FD_ZERO(&readfds);
       FD_SET(a,&readfds);
       FD_SET(b,&readfds);
       int maxfd=(a>b?a:b)+1;
       int ready=select(maxfd,&readfds,nullptr,nullptr,nullptr);
       if(ready<=0){
        break;
       }
       if(FD_ISSET(a,&readfds)){
        int n=recv(a,buffer,sizeof(buffer),0);
        if(n<=0){break;}
        send(b,buffer,n,0);
       }
       if(FD_ISSET(b,&readfds)){
        int n=recv(b,buffer,sizeof(buffer),0);
        if(n<=0){break;}
        send(a,buffer,n,0);
       }
    }
}
SOCKET open_connection(const std::string &host, int port) {
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "[-] Socket creation failed\n";
        return INVALID_SOCKET;
    }
    set_timeouts(server_socket);
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

bool forward_request(SOCKET client_socket, const HttpRequest &request) {

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

bool handle_connect(SOCKET client_socket,const HttpRequest &req){
    set_timeouts(client_socket);
if(isblocked(req.host)){
    const char *resp=
         "HTTP/1.1 403 Forbidden\r\n"
         "Content-Length: 0\r\n\r\n";
    send(client_socket,resp,(int)strlen(resp),0);
    return false;     
     
}

SOCKET server_socket=open_connection(req.host,req.port);
if(server_socket==INVALID_SOCKET)return false;
const char *ok="HTTP/1.1 200 Connection Established\r\n\r\n";
send(client_socket,ok,(int)strlen(ok),0);
tunnel(client_socket,server_socket);
closesocket(server_socket);
closesocket(client_socket);
return true;


}