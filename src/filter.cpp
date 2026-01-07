#include "../include/filter.h"
#include <unordered_set>
#include <algorithm>
#include <fstream>
#include <string>
#include <iostream>

using namespace std;

static unordered_set<string> blocked;

string normalize(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

void load_blocklist(const string &filename) {
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        if (!line.empty() && line[0] != '#'){
            blocked.insert(normalize(line));}
    }
}

bool isblocked(const string &host) {
    string h=normalize(host);
    for(const auto &b:blocked){
        if(h==b)return true;
        if(h.size()>b.size()&&h.compare(h.size()-b.size(),b.size(),b)==0 && h[h.size()-b.size()-1]=='.')//subdomain match 
        {
            return true;
        }
    }
    return false;
}
