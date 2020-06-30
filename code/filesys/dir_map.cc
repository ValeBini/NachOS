#include "dir_map.hh"

DirData::DirData(unsigned sector){
    std::string dirName = string("DirLock ") + std::to_string(sector);
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
    dirTable = new std::map<unsigned,DirData*>;
}

DirData *
DirMap::GetDirData(unsigned sector){
    if(!dirTable->count(sector))
        return nullptr;
    return (*dirTable)[sector];
}

void
DirMap::Add(unsigned sector){
    ASSERT(!dirTable->count(sector));
    DirData * data = new DirData(sector);
    dirTable->insert(std::make_pair(sector,data));
}

