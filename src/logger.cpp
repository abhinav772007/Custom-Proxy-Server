#include "../include/logger.h"
#include<string>
#include <ctime>
#include<sys/stat.h>
#include <fstream>
#include <iomanip>
#include <sstream>
using namespace std;

#include<iostream>


static string logfile="../logs/proxy.log";

static const size_t maxsize=200;

static string currenttime(){
    time_t now=time(nullptr);
    tm *it=localtime(&now);
    std::ostringstream out;
    out<<put_time(it,"%Y-%m-%d %H:%M:%S");
    return out.str();
    
}
static void rotatelog(){
    struct stat st;
    if(stat(logfile.c_str(),&st)==0){
        if(st.st_size>=maxsize){
            rename(logfile.c_str(),"../logs/proxy.log.old");
        }
    }
}

void logreq(const std::string &clientadd,const std::string &serveradd,const std::string &requestline,const std::string &action){
    rotatelog();
    ofstream log(logfile,std::ios::app);
    if(!log.is_open()){
        return ;
    }
    log<<currenttime()<<" | "<<clientadd<<" | "<<serveradd<<" | "<<requestline<<" | "<<action<<"\n";
}




