#include "../include/config.h"
#include <fstream>
#include <string>
static int port=8888;
static std::string bind_addr="0.0.0.0";
static void load_config(){
    std::ifstream f("../config/proxy.conf");
    std::string line;
    while(getline(f,line)){
        if(line.find("PORT=")==0){port=stoi(line.substr(5));}
        else if(line.find("BIND=")==0){
            bind_addr=line.substr(5);
        }
    }
}

int get_port(){
    static bool loaded=false;
    if(!loaded){
        load_config();
        loaded=true;
    }
    return port;
}
std::string get_bind_address(){
    static bool loaded=false;
    if(!loaded){
        load_config();
        loaded=true;
    }
    return bind_addr;
}