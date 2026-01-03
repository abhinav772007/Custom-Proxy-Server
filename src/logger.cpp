#include "../include/logger.h"
#include<string>
#include <ctime>
#include<sys/stat.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include<vector>
#ifdef _WIN32
#include <direct.h>
#endif
using namespace std;
#include<iostream>
static string logfile="logs/proxy.log";
static const size_t maxsize=4096;

static string currenttime(){
    time_t now=time(nullptr);
    tm *it=localtime(&now);
    std::ostringstream out;
    out<<put_time(it,"%Y-%m-%d %H:%M:%S");
    return out.str();
    
}
static void ensure_log_dir() {
    #ifdef _WIN32
        _mkdir("logs");
    #else
        mkdir("logs", 0755);
    #endif
    }
    
static void rotatelog(){
    ifstream in(logfile,ios::binary | ios::ate);
    if(!in.is_open())return ;
    size_t size=in.tellg();
    if(size<=maxsize){
        in.close();
        return;
    }
    size_t present=maxsize;
    in.seekg(size-present);
    vector<char> buffer(present);
    in.read(buffer.data(),present);
    in.close();

    ofstream out(logfile,ios::binary | ios::trunc);
    out.write(buffer.data(),present);
    out.close();
}

void logreq(const std::string &clientadd,const std::string &serveradd,const std::string &requestline,const std::string &action,int statuscode,size_t bytes_transferred){
    ensure_log_dir();
    rotatelog();
    ofstream log(logfile,std::ios::app);
    if(!log.is_open()){
        cerr<<"[Logger] failed to open\n";
        return ;
    }
    log<<currenttime()<<" | "<<clientadd<<" | "<<serveradd<<" | "<<requestline<<" | "<<action<<" | "<<statuscode<<" | "<<bytes_transferred<<"\n";
}
