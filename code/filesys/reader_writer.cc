#include "reader_writer.hh"
#include <cstdio>

ReaderWriter::ReaderWriter(const char* name){
    int n = strlen(name);

    char lname [n+15];
    
    snprintf(lname, n+11, "%s WriterLock", name);
    writerLock = new Lock(lname);

    snprintf(lname, n+13, "%s rCounterLock", name);
    rCounterLock = new Lock(lname);

    snprintf(lname, n+14, "%s noReadersCond", name);
    noReaders = new Condition(lname, rCounterLock);

    rCounter = 0;
}

ReaderWriter::~ReaderWriter(){
    delete writerLock;
    delete rCounterLock;
    delete noReaders;
}

void 
ReaderWriter::WriterAcquire(){
    rCounterLock->Acquire();
    while (rCounter > 0) noReaders->Wait();
    writerLock->Acquire();
}

void
ReaderWriter::WriterRelease(){
    writerLock->Release();
    rCounterLock->Release();
}

void
ReaderWriter::ReaderAcquire(){
    rCounterLock->Acquire();
    rCounter++;
    rCounterLock->Release();
}

void
ReaderWriter::ReaderRelease(){
    rCounterLock->Acquire();
    rCounter--;
    if (rCounter == 0) noReaders->Signal();
    rCounterLock->Release();
}
