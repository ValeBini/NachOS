#include "reader_writer.hh"
#include <map>

class MetaData{
    public: 
        int links;
        Lock* linkLock;
        ReaderWriter* rw; 
        
        MetaData(const char * name){
            links = 0;
            linkLock = new Lock("MetaData Link Lock");
            rw = new ReaderWriter(name);
        }

        ~MetaData(){
            delete linkLock;
            delete rw;
        }

        void IncrementLinks(){
            linkLock->Acquire();
            links++;
            linkLock->Release();
        }
        
        bool DecrementLinks(){
            linkLock->Acquire();
            links--;
            if(links == 0){
                linkLock->Release();
                return false; // Hay que Borrarlo.    
            }
            linkLock->Release();
            return true;
        }
};


struct cmp_str
{
   bool operator()(char const *a, char const *b) const
   {
      return strcmp(a, b) < 0;
   }
};


#include <map>
// using namespace std;
class OpenFilesMap{
private:
    std::map<const char *,MetaData*,cmp_str> * metaData;
    
public:
    OpenFilesMap(){
        metaData = new std::map<const char*, MetaData*,cmp_str>;
    }
    ~OpenFilesMap(){ 
        delete metaData;   
    }

    void Open(const char * name){
        if(metaData->count(name)==0){      //Primera vez que se habre el archivo
            MetaData* temp = new MetaData(name);
            temp->IncrementLinks();
            metaData->insert(std::make_pair(name,temp)); 
        }else{                              //Alguien ya abrio el archivo
            (*metaData)[name]->IncrementLinks();
        }
        
    }
    
    void Close(const char * name){
        bool readyToBeDeleted = (*metaData)[name]->DecrementLinks();
        if(readyToBeDeleted){
            delete (*metaData)[name];
            metaData->erase(name);
        }
    }
/*
    void Remove(char * name){
        bool readyToBeDeleted = (*metaData)[name]->DecrementLinks();
        if(radyToBeDelete){
            
        }
    }
*/
    ReaderWriter* GetRW(const char* name){
        return (*metaData)[name]->rw;
    }
};
