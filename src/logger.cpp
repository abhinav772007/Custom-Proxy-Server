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
#include <unordered_map>
#include<chrono>
using namespace std::chrono;
static size_t total_reqs=0;
static unordered_map<string,size_t>count;
static auto metrics_start=std::chrono::steady_clock::now();
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
    total_reqs++;
    count[serveradd]++;
}
void print_metrics(){
    using namespace std::chrono;
    auto now=steady_clock::now();
    auto dur=duration_cast<seconds>(now-metrics_start).count();
    if(dur==0)dur=1;
    cout<<"\nProxy Metrics\n";
    cout<<"Total requests: "<<total_reqs<<"\n";
    cout<<"Requests per minute: "<<(total_reqs*60)/dur<<"\n";
    cout<<"Top hosts:\n";
    for(const auto &p : count){
        cout<<" "<<p.first<<" : "<<p.second<<"\n";
    }
    cout<<"\n";
}
