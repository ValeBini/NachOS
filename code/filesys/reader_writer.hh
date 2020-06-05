#ifndef FYLESYSTEM_READER_WRITER
#define FYLESYSTEM_READER_WRITER

#include "threads/synch.hh"
#include <string.h>

class Lock;
class Condition;

class ReaderWriter{
    public:
        ReaderWriter(const char* name);
        
        ~ReaderWriter();

        void WriterAcquire();
        void WriterRelease();
        void ReaderAcquire();
        void ReaderRelease();

    private:
        int rCounter;
        Lock* rCounterLock;
        Condition* noReaders;
};

#endif
