#include "dir_map.hh"

DirData::DirData(unsigned size_, std::string dirName){
    size = size_;
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
DirData::SetSize(unsigned newSize){
    size = newSize;
}

unsigned
DirData::GetSize(){
    return size;
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
DirMap::Update(std::string dirName,unsigned size){
    if(!dirTable->count(dirName)){
        DirData * data = new DirData(size,dirName);
        dirTable->insert(std::make_pair(dirName,data));
    } else {
        (*dirTable)[dirName]->SetSize(size);
    }
}