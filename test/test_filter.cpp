#include <iostream>
#include "../include/filter.h"
using namespace std;

int main(){
    load_blocklist("../config/blocked_domains.txt");
cout<<"example.com blocked?"<<isblocked("example.com")<<endl;
cout<<"google.com blocked?"<<isblocked("google.com")<<endl;
}
