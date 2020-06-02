#include "synch.hh"
#include <string.h>


class ReaderWriter{
    public:
        ReaderWriter(const char* name);
        
        ~ReaderWriter();

        void WriterAcquire();
        void WriterRelease();
        void ReaderAcquire();
        void ReaderRelease();

    private:
        Lock* writerLock;
        int rCounter;
        Lock* rCounterLock;
        Condition* noReaders;
};
