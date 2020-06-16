



#include "filesys/path.hh" 
// #include <string>
// #include <sstream>
// #include <vector>
// #include <list>
// #include <iostream>

using namespace std;

void
Path::Set(string newPath){
    
    path.clear();

    stringstream check1(newPath); 

    string intermediate; 

    while(getline(check1, intermediate, '/')) 
    { 
        if(intermediate != "")
            path.push_back(intermediate); 
        
    } 
}


Path::Path(string newPath){
    Set(newPath);
}

Path::~Path(){}

string
Path::FromPathToStr(){
string res = "";;
    if(path.empty()) return "/";
    for(string s:path)
        res+=s+"/";
    return res;
}


void
Path::Merge(string cdPath){

    if(cdPath[0] == '/'){
        Set(cdPath);
    } else {

        stringstream check1(cdPath); 

        string name; 

        while(getline(check1,name, '/')) 
        { 
            if(name != ""){
                if(name == ".."){
                    if(!path.empty())
                         path.pop_back();
                } else {
                    path.push_back(name);
                }
            }
            
        } 

    }

}
