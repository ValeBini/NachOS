#ifndef DIR_MAP
#define DIR_MAP

#include <string>
#include <map>
#include "threads/synch.hh"

class Lock;

class DirData {
    public:
        DirData(std::string dirName);
        ~DirData();
        
        void AcquireL();
        void ReleaseL();
        
    private:
        Lock* lock;
        char * lockName;
};


class DirMap{

    private:
        std::map<std::string,DirData*> * dirTable;

    public:
        DirMap();
        ~DirMap();
        DirData * GetDirData(string path);
        void Add(std::string dirName);
};

#endif