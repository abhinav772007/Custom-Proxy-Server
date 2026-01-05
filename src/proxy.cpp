#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include "../include/logger.h"
#include "../include/filter.h"
#include "../include/parser.h"
#include "../include/forwarder.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;
#define PROXY_PORT 8888
int main() {
    load_blocklist("config/blocked_domains.txt");
    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2,2),&wsa)!=0){
        cerr<<"WSAStartup failed\n";
        return 1;
    }

    SOCKET listen_socket=socket(AF_INET,SOCK_STREAM,0);
    if(listen_socket==INVALID_SOCKET){
        cerr<<"Socket creation failed\n";
        WSACleanup();
        return 1;
    }
sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PROXY_PORT);
   if(bind(listen_socket,(sockaddr*)&addr,sizeof(addr))==SOCKET_ERROR){
        cerr<<"Bind failed\n";
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    if(listen(listen_socket, SOMAXCONN)==SOCKET_ERROR){
        cerr<<"Listen failed\n";
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    cout<<"Proxy listening on port "<< PROXY_PORT<<endl;

    while(true){
        SOCKET client_socket = accept(listen_socket, nullptr, nullptr);
        if(client_socket==INVALID_SOCKET)
            continue;

        char buffer[8192];
        int received=recv(client_socket,buffer,sizeof(buffer),0);
        if (received<=0){
            closesocket(client_socket);
            continue;
        }
        string raw_request(buffer, received);

        HttpRequest request = getdetails(raw_request);
        request.raw = raw_request;
        
        string reqline = request.method + " " + request.path + " " + request.version;
        
        bool blocked = isblocked(request.host);
        string action = blocked ? "BLOCKED" : "ALLOWED";
        
        int status = blocked ? 403 : 200; //log before forwarding

         logreq(
    "127.0.0.1",
    request.host + ":" + std::to_string(request.port),
    reqline,
    action,
    status,
    0
);

        
        if(blocked){
            const char *resp =
                "HTTP/1.1 403 Forbidden\r\n"
                "Content-Length: 0\r\n"
                "Connection: close\r\n\r\n";
            send(client_socket, resp, strlen(resp), 0);
        }else{
            forward_request(client_socket, request);
        }
        
        
        closesocket(client_socket);
    }

    closesocket(listen_socket);
    WSACleanup();
    return 0;
}
