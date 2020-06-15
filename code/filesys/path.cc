#include "filesys/path.hh" 
// #include <string>
// #include <sstream>
// #include <vector>
// #include <list>
// #include <iostream>

using namespace std;

Path::Path(string newPath){
  
    stringstream check1(newPath); 

    string intermediate; 

    while(getline(check1, intermediate, '/')) 
    { 
        if(intermediate != "")
            path.push_back(intermediate); 
        
    } 
}

Path::~Path(){}

string
Path::fromPathToStr(){
string res = "";;
    if(path.empty()) return "/";
    for(string s:path)
        res+=s+"/";
    return res;
}


