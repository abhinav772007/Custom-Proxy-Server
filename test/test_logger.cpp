#include "../include/logger.h"
#include <iostream>
using namespace std;
int main(){
logreq("127.0.0.1:54321",
        "example.com:80",
        "GET / HTTP/1.1",
        "ALLOWED");
      logreq(
            "127.0.0.1:1",
            "example.com:77",
            "GET / HTTP/1.1",
            "ALLOWED"
        );
      cout<<"logger test complete";
}