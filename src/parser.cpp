#include "../include/parser.h"
#include <sstream>
#include<string>
using namespace std;

HttpRequest getdetails(const std::string &request){

    HttpRequest temp;
    temp.raw=request; // stores complete request
    temp.length=0;
    temp.port=80;
    
    std::istringstream requestStream(request);
    std::string line;
    if(getline(requestStream,line)){
        if(!line.empty() && line.back()=='\r'){
            line.pop_back();
        }
        std::istringstream lineStream(line);
        string url;
        lineStream>>temp.method>>url>>temp.version;
        if(url.substr(0,7)=="http://"){
        url=url.substr(7);
    }
    else if(url.substr(0,8)=="https://"){
    url=url.substr(8);
    }
    size_t position=url.find('/');
    if(position!=std::string::npos){
        temp.host=url.substr(0,position);
        temp.path=url.substr(position);
    }
    else{
        temp.host=url;
        temp.path="/";
    }
    }
    // headers
    while(getline(requestStream,line)){
        if(!line.empty()&& line.back()=='\r'){
            line.pop_back();
        }
        if(line.empty()){break;}
        if(line.substr(0,6)=="Host: "){
            if(temp.host.empty()){
                temp.host=line.substr(6);
            }}
            else if(line.substr(0,14)=="Content-Type: "){
                temp.type=line.substr(14);
            }
            else if(line.substr(0,16)=="Content-Length: "){
                temp.length=stoi(line.substr(16));
            }
        
    }

    //port
    size_t colon=temp.host.find(':');
    if(colon!=string::npos){
        temp.port=stoi(temp.host.substr(colon+1));
        temp.host=temp.host.substr(0,colon);
    }
    //body
    if(temp.length>0){
        temp.body.resize(temp.length);
        requestStream.read(&temp.body[0],temp.length);
    }

    return temp;


}
