#include "dir_map.hh"

DirData::DirData(std::string dirName){
    dirName = string("DirLock ") + dirName;
    lockName = new char [dirName.size()];
    strcpy(lockName, dirName.c_str());
    lock = new Lock(lockName);
}

DirData::~DirData(){
    delete lock;
    delete lockName;
}


void
DirData::AcquireL(){
    lock->Acquire();
}

void
DirData::ReleaseL(){
    lock->Release();
}



DirMap::~DirMap(){
    delete dirTable;
}

DirMap::DirMap(){
    dirTable = new std::map<std::string,DirData*>;
}

DirData *
DirMap::GetDirData(std::string dirName){
    if(!dirTable->count(dirName))
        return nullptr;
    return (*dirTable)[dirName];
}

void
DirMap::Add(std::string dirName){
    ASSERT(!dirTable->count(dirName));
    DirData * data = new DirData(dirName);
    dirTable->insert(std::make_pair(dirName,data));
}

