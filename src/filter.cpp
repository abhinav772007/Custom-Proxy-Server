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
    return blocked.count(normalize(host));
}
