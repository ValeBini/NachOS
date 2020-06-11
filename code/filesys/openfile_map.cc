#include "openfile_map.hh"
#include "threads/system.hh"


MetaData::MetaData(const char * name){
    links = 0;
    linkLock = new Lock("MetaData Link Lock");
    rw = new ReaderWriter(name);
    removed = false;
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
        return true; // Hay que Borrarlo.    
    }
    linkLock->Release();
    return false;
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
    mapLock = new Lock("mapLock");
}

OpenFilesMap::~OpenFilesMap(){ 
    delete metaData;   
    delete mapLock;
}

void 
OpenFilesMap::Open(const char * name){
    DEBUG('F',"INCIO OPEN %s -----------\n", name);
    mapLock->Acquire();
    if(metaData->count(name)==0){      //Primera vez que se habre el archivo
        DEBUG('F',"Open %s : Primera vez\n", name);
        MetaData* temp = new MetaData(name);
        temp->IncrementLinks();
        metaData->insert(std::make_pair(name,temp)); 
    }else{                              //Alguien ya abrio el archivo
        DEBUG('F',"Open %s : Archivo sin nombre\n",name);
        (*metaData)[name]->IncrementLinks();
    }
    mapLock->Release();
    DEBUG('F',"FIN OPEN %s -----------\n", name);
}

void 
OpenFilesMap::Close(const char * name){
    DEBUG('F',"INCIO CLOSE %s -----------\n", name);

    mapLock->Acquire();
    bool remove = false;
    bool readyToBeDeleted = (*metaData)[name]->DecrementLinks();
    if(readyToBeDeleted){
        DEBUG('F',"Close %s : Se borra ENTRADA en mapa.\n",name);
        if((*metaData)[name]->removed) remove = true;
        delete (*metaData)[name];
        metaData->erase(name);
        if (remove) {
            DEBUG('F',"Close %s : Se borra ARCHIVO.\n", name);
            mapLock->Release();
            fileSystem->Remove(name);
        } else mapLock->Release();
    } else mapLock->Release();
    
    DEBUG('F',"FIN CLOSE %s -----------\n", name);
}

bool 
OpenFilesMap::Remove(const char * name){
    DEBUG('F',"INCIO REMOVE %s -----------\n", name);
    
    if(metaData->count(name)==0){
        DEBUG('F',"Remove %s : NO Esta abierto por ningún programa.\n",name);
        DEBUG('F',"FIN REMOVE %s -----------\n", name);
        
        return true;
    } else {
        DEBUG('F',"Remove %s : Esta abierto por algún programa.\n",name);
        (*metaData)[name]->removed = true;
        DEBUG('F',"FIN REMOVE %s -----------\n", name);
        
        return false;
    }
    
}

ReaderWriter* 
OpenFilesMap::GetRW(const char* name){
    return (*metaData)[name]->rw;
}

bool
OpenFilesMap::checkNotRemoved(const char * name){
    mapLock->Acquire();
    if(metaData->count(name)==0){
        DEBUG('F',"Checking if not removed.%s. Not open files map. Remove\n",name);
        mapLock->Release();
        return true;
    }
    if(!(*metaData)[name]->removed){
        DEBUG('F',"Checking if not removed.%s. Ready to be removed\n",name);
    }else{

        DEBUG('F',"Checking if not removed.%s. Not ready to be removed\n",name);
    }
    mapLock->Release();
    return !(*metaData)[name]->removed;
}
