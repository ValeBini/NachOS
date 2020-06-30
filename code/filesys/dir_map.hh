#ifndef DIR_MAP
#define DIR_MAP

#include <string>
#include <map>
#include "threads/synch.hh"

class Lock;

class DirData {
    public:
        DirData(unsigned sector);
        ~DirData();
        
        void AcquireL();
        void ReleaseL();
        
    private:
        Lock* lock;
        char * lockName;
};


class DirMap{

    private:
        std::map<unsigned,DirData*> * dirTable;

    public:
        DirMap();
        ~DirMap();
        DirData * GetDirData(unsigned sector);
        void Add(unsigned sector);
};

#endif