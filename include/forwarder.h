#ifndef FORWARDER_H
#define FORWARDER_H
#include "parser.h"
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

bool forward_request(SOCKET client_sock, const HttpRequest &req);// true on success and false on failures
bool handle_connect(SOCKET client_sock,const HttpRequest &req);
#endif
