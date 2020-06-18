#ifndef DIR_MAP
#define DIR_MAP

#include <string>
#include <map>
#include "threads/synch.hh"

class Lock;

class DirData {
    public:
        DirData(unsigned size_, std::string dirName);
        ~DirData();
        
        void SetSize(unsigned newSize);
        unsigned GetSize();
        
        void AcquireL();
        void ReleaseL();
        
    private:
        unsigned size;
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
        void Update(string dirName, unsigned size);
};

#endif