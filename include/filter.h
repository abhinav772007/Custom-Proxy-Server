#ifndef FILTER_H
#define FILTER_H
#include <string>

void load_blocklist(const std::string &file);
bool isblocked(const std::string &host);

#endif