#include "../include/parser.h"
#include <sstream>
#include<string>
using namespace std;
static bool starts_with_ci(const std::string &line,const std::string &key){
if(line.size()<key.size())return false;
for(size_t i=0;i<key.size();i++){
    if(tolower(line[i])!=tolower(key[i]))return false;
}
return true;
}
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
        if(url.rfind("http://",0)==0){
        url=url.substr(7);
    }
    else if(url.rfind("https://",0)==0){
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
        if(starts_with_ci(line,"Host: ")){
            if(temp.host.empty()){
                temp.host=line.substr(6);
            }}
            else if(starts_with_ci(line,"Content-Type: ")){
                temp.type=line.substr(14);
            }
            else if(starts_with_ci(line,"Content-Length: ")){
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
