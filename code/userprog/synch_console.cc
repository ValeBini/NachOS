#include "synch_console.hh"

SynchConsole::SynchConsole(const char *readFile, const char *writeFile){
    console = new Console(readFile, writeFile, ReadAvail, WriteDone, this);
    readerLock = new Lock("Reader Lock");
    writerLock = new Lock("Writer Lock");
    readerSem = new Semaphore("Reader Semaphore", 0);
    writerSem = new Semaphore("Writer Semaphore", 0);
}

SynchConsole::~SynchConsole()
{
    delete readerLock;
    delete writerLock;
    delete readerSem;
    delete writerSem;
    delete console;
}

void SynchConsole::PutChar(char ch){
    writerLock -> Acquire();
    console -> PutChar(ch);
    // Se asegura de que se haya escrito el caracter para poder
    // continuar.
    writerSem -> P();
    writerLock -> Release();
}

char SynchConsole::GetChar(){
    readerLock -> Acquire();
    // Se asegura de que haya un caracter para leer.
    readerSem -> P();
    char returnValue = console -> GetChar();
    readerLock -> Release();
    return returnValue;
}


// Llamado por CheckCharAvail() en console.
void SynchConsole::ReadAvail(void * data){
    ((SynchConsole *)data)->readerSem->V();
}

// Llamado por PutChar() luego de escribir el caracter.
void SynchConsole::WriteDone(void * data){
    ((SynchConsole *)data)->writerSem->V();
}



