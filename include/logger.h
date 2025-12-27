#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <cstddef>

void logreq(const std::string &clientadd,const std::string &serveradd,const std::string &requestline,const std::string &action,int statuscode,size_t bytes_transferred);

#endif