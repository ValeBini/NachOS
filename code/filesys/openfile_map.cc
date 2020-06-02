#include "openfile_map.hh"


MetaData::MetaData(const char * name){
    links = 0;
    linkLock = new Lock("MetaData Link Lock");
    rw = new ReaderWriter(name);
}

MetaData::~MetaData(){
    delete linkLock;
    delete rw;
}

void 
MetaData::IncrementLinks(){
    linkLock->Acquire();
    links++;
    linkLock->Release();
}

bool 
MetaData::DecrementLinks(){
    linkLock->Acquire();
    links--;
    if(links == 0){
        linkLock->Release();
        return false; // Hay que Borrarlo.    
    }
    linkLock->Release();
    return true;
}


// struct cmp_str
// {
//    bool operator()(char const *a, char const *b) const
//    {
//       return strcmp(a, b) < 0;
//    }
// };


OpenFilesMap::OpenFilesMap(){
    metaData = new std::map<const char*, MetaData*,cmp_str>;
}
OpenFilesMap::~OpenFilesMap(){ 
    delete metaData;   
}

void 
OpenFilesMap::Open(const char * name){
    if(metaData->count(name)==0){      //Primera vez que se habre el archivo
        MetaData* temp = new MetaData(name);
        temp->IncrementLinks();
        metaData->insert(std::make_pair(name,temp)); 
    }else{                              //Alguien ya abrio el archivo
        (*metaData)[name]->IncrementLinks();
    }
    
}

void 
OpenFilesMap::Close(const char * name){
    bool readyToBeDeleted = (*metaData)[name]->DecrementLinks();
    if(readyToBeDeleted){
        delete (*metaData)[name];
        metaData->erase(name);
    }
}
/*
void 
OpenFilesMap::Remove(char * name){
    bool readyToBeDeleted = (*metaData)[name]->DecrementLinks();
    if(radyToBeDelete){
        
    }
}
*/
ReaderWriter* 
OpenFilesMap::GetRW(const char* name){
    return (*metaData)[name]->rw;
}
