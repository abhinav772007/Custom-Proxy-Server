#ifndef PARSER_H
#define PARSER_H
#include <string>
struct HttpRequest{
    std::string raw;
    std::string method;
    std::string host;
    std::string path;
    std::string version;
    std::string type;
    std::string body;
    int length;
    int port;
};

HttpRequest getdetails(const std::string &request);

#endif