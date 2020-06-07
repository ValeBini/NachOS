#ifndef OPENFILE_MAP
#define OPENFILE_MAP

#include "reader_writer.hh"
#include "threads/synch.hh"

#include <map>
#include <string.h>

class Lock;
class ReaderWriter;

class MetaData{
    public: 
        int links;
        Lock* linkLock;
        ReaderWriter* rw; 
        bool removed;
        
        MetaData(const char * name);

        ~MetaData();

        void IncrementLinks();
        
        bool DecrementLinks();
};


struct cmp_str
{
   bool operator()(char const *a, char const *b) const
   {
      return strcmp(a, b) < 0;
   }
};


// #include <map>
// using namespace std;
class OpenFilesMap{
private:
    std::map<const char *,MetaData*,cmp_str> * metaData;
    
public:
    OpenFilesMap();
    ~OpenFilesMap();

    Lock* mapLock;

    void Open(const char * name);
    
    void Close(const char * name);

    bool Remove(const char * name);

    ReaderWriter* GetRW(const char * name);

    bool checkNotRemoved(const char * name);
};

#endif