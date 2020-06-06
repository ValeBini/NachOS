#include "reader_writer.hh"
#include <cstdio>

ReaderWriter::ReaderWriter(const char* name){
    int n = strlen(name);

    char lname [n+15];
    
    snprintf(lname, n+13, "%s rCounterLock", name);
    rCounterLock = new Lock(lname);

    snprintf(lname, n+14, "%s noReadersCond", name);
    noReaders = new Condition(lname, rCounterLock);

    rCounter = 0;
}

ReaderWriter::~ReaderWriter(){
    delete rCounterLock;
    delete noReaders;
}

void 
ReaderWriter::WriterAcquire(){
    DEBUG('W', "WriterAcquire %s \n",rCounterLock->GetName());
    if(!rCounterLock->IsHeldByCurrentThread()) rCounterLock->Acquire();
    while (rCounter > 0) noReaders->Wait();
}

void
ReaderWriter::WriterRelease(){
    DEBUG('W', "WriterRelease %s \n",rCounterLock->GetName());
    noReaders->Signal();
    rCounterLock->Release();
}

void
ReaderWriter::ReaderAcquire(){
    DEBUG('W', "ReaderAcquire %s \n",rCounterLock->GetName());
    if(!(rCounterLock->IsHeldByCurrentThread())){
        DEBUG('W', "IsNotHeldByCurrentThread \n");
        rCounterLock->Acquire();
        rCounter++;
        rCounterLock->Release();
    }
}

void
ReaderWriter::ReaderRelease(){
    DEBUG('W',"Release Reader. %s\n",rCounterLock->GetName());
    if(!(rCounterLock->IsHeldByCurrentThread())){
        DEBUG('W', "IsNotHeldByCurrentThread \n");
        rCounterLock->Acquire();
        rCounter--;
        if (rCounter == 0) noReaders->Broadcast();
        rCounterLock->Release();
    }
}
