/// Simple test routines for the file system.
///
/// We implement:
///
/// Copy
///     Copy a file from UNIX to Nachos.
/// Print
///     Cat the contents of a Nachos file.
/// Perftest
///     A stress test for the Nachos file system read and write a really
///     really large file in tiny chunks (will not work on baseline system!)
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2016-2020 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "file_system.hh"
#include "lib/utility.hh"
#include "machine/disk.hh"
#include "machine/statistics.hh"
#include "threads/thread.hh"
#include "threads/system.hh"

// #include "filesys/file_header.cc"
// #include "filesys/directory.cc"
#include "filesys/file_system.hh"

#ifdef DIR
#include "filesys/path.hh"
#endif

#include <stdio.h>
#include <string.h>

#include <string>

using namespace std;

//#define TEST_EJ1
#define TEST_DIRECTORIES


static const unsigned TRANSFER_SIZE = 10;  // Make it small, just to be
                                           // difficult.

/// Copy the contents of the UNIX file `from` to the Nachos file `to`.
void
Copy(const char *from, const char *to)
{
    ASSERT(from != nullptr);
    ASSERT(to != nullptr);

    // Open UNIX file.
    FILE *fp = fopen(from, "r");
    if (fp == nullptr) {
        printf("Copy: could not open input file %s\n", from);
        return;
    }

    // Figure out length of UNIX file.
    fseek(fp, 0, 2);
    int fileLength = ftell(fp);
    fseek(fp, 0, 0);

    DEBUG('f', "Copying file %s, size %u, to file %s\n",
          from, fileLength, to);

    // Create a Nachos file of the same length.

    #ifdef DIR
    int last = 0;
    string path = "";
    if(to[0]=='/'){
        for(int i = 0; to[i]; i++) if(to[i] == '/') last = i;
        for(int i = 0; i<last; i++) path += to[i];
        last++;
    }
    if (!fileSystem->Create(to+last,fileLength,path)) {  // Create Nachos file.
        printf("Copy: could not create output file %s\n", to);
        fclose(fp);
        return;
    }

    #else

    if (!fileSystem->Create(to, fileLength)) {  // Create Nachos file.
        printf("Copy: could not create output file %s\n", to);
        fclose(fp);
        return;
    }

    #endif



#ifdef DIR
    OpenFile *openFile = fileSystem->Open(to+last,path);
#else
    OpenFile *openFile = fileSystem->Open(to);
#endif
    ASSERT(openFile != nullptr);

    // Copy the data in `TRANSFER_SIZE` chunks.
    char *buffer = new char [TRANSFER_SIZE];
    int amountRead;
    while ((amountRead = fread(buffer, sizeof(char),
                               TRANSFER_SIZE, fp)) > 0)
        openFile->Write(buffer, amountRead);
    
    delete [] buffer;

    // Close the UNIX and the Nachos files.
    delete openFile;
    fclose(fp);
}

/// Print the contents of the Nachos file `name`.
void
Print(const char *name)
{
    ASSERT(name != nullptr);

    OpenFile *openFile = fileSystem->Open(name);
    if (openFile == nullptr) {
        fprintf(stderr, "Print: unable to open file %s\n", name);
        return;
    }

    char *buffer = new char [TRANSFER_SIZE];
    int amountRead;
    while ((amountRead = openFile->Read(buffer, TRANSFER_SIZE)) > 0)
        for (unsigned i = 0; i < (unsigned) amountRead; i++)
            printf("%c", buffer[i]);

    delete [] buffer;
    delete openFile;  // close the Nachos file
}


/// Performance test
///
/// Stress the Nachos file system by creating a large file, writing it out a
/// bit at a time, reading it back a bit at a time, and then deleting the
/// file.
///
/// Implemented as three separate routines:
/// * `FileWrite` -- write the file.
/// * `FileRead` -- read the file.
/// * `PerformanceTest` -- overall control, and print out performance #'s.

static const char FILE_NAME[] = "TestFile";
static const char CONTENTS[] = "1234567890";
#ifdef TEST_EJ1
static const unsigned CONTENT_SIZE = 1;
static const unsigned NUMBEROFTHREADS = 10;
static const unsigned TIMES = 10;
static const unsigned FILE_SIZE = CONTENT_SIZE * TIMES * NUMBEROFTHREADS;
#else
static const unsigned CONTENT_SIZE = sizeof CONTENTS - 1;
static const unsigned FILE_SIZE = CONTENT_SIZE * 5000;
#endif


#ifdef TEST_EJ1

static void
FileWrite(char * c)
{
    printf("Sequential write of %u byte file, in %u byte chunks\n",
           FILE_SIZE, CONTENT_SIZE);

    OpenFile *openFile = fileSystem->Open(FILE_NAME);
    if (openFile == nullptr) {
        fprintf(stderr, "Perf test: unable to open %s\n", FILE_NAME);
        return;
    }
    openFile->Seek(((*c)-'A')* 10);
    for (unsigned i = 0; i < TIMES ; i++) {
        int numBytes = openFile->Write(c, CONTENT_SIZE);
        if (numBytes < int(CONTENT_SIZE)) {
            fprintf(stderr, "Perf test: UNABLE TO WRITE %s\n", FILE_NAME);
            break;
        }
    }

    delete openFile;
}

static void
FileRead()
{
    printf("Sequential read of %u byte file, in %u byte chunks\n",
           FILE_SIZE, CONTENT_SIZE);

    OpenFile *openFile = fileSystem->Open(FILE_NAME);
    if (openFile == nullptr) {
        fprintf(stderr, "Perf test: unable to open file %s\n", FILE_NAME);
        return;
    }

    char *buffer = new char [CONTENT_SIZE];
    for (unsigned i = 0; i < FILE_SIZE; i += CONTENT_SIZE) {
        int numBytes = openFile->Read(buffer, CONTENT_SIZE);
        buffer[1] = 0;
        // fprintf(stderr,"Read: %s\n",buffer);
        //  if (numBytes < int(CONTENT_SIZE)) {
        //      printf("Perf test: unable to read %s\n", FILE_NAME);
        //      break;
        //  }
    }

    delete [] buffer;
    delete openFile;
}

void simpleThread(void * c){
    printf("Starting file system performance test: %s\n",(char *)c);
    FileWrite((char *) c);
    FileRead();
    if (!fileSystem->Remove(FILE_NAME)) {
        printf("Perf test: unable to remove %s\n", FILE_NAME);
        return;
    }
    printf("Ending file system performance test\n");
}

#else 

static void
FileWrite()
{
    printf("Sequential write of %u byte file, in %u byte chunks\n",
           FILE_SIZE, CONTENT_SIZE);

    if (!fileSystem->Create(FILE_NAME, FILE_SIZE)) {
        fprintf(stderr, "Perf test: cannot create %s\n", FILE_NAME);
        return;
    }

    OpenFile *openFile = fileSystem->Open(FILE_NAME);
    if (openFile == nullptr) {
        fprintf(stderr, "Perf test: unable to open %s\n", FILE_NAME);
        return;
    }

    for (unsigned i = 0; i < FILE_SIZE; i += CONTENT_SIZE) {
        int numBytes = openFile->Write(CONTENTS, CONTENT_SIZE);
        if (numBytes < 10) {
            fprintf(stderr, "Perf test: unable to write %s\n", FILE_NAME);
            break;
        }
    }

    delete openFile;
}

static void
FileRead()
{
    printf("Sequential read of %u byte file, in %u byte chunks\n",
           FILE_SIZE, CONTENT_SIZE);

    OpenFile *openFile = fileSystem->Open(FILE_NAME);
    if (openFile == nullptr) {
        fprintf(stderr, "Perf test: unable to open file %s\n", FILE_NAME);
        return;
    }

    char *buffer = new char [CONTENT_SIZE];
    for (unsigned i = 0; i < FILE_SIZE; i += CONTENT_SIZE) {
        int numBytes = openFile->Read(buffer, CONTENT_SIZE);
        if (numBytes < CONTENT_SIZE || strncmp(buffer, CONTENTS, CONTENT_SIZE)) {
            printf("Perf test: unable to read %s\n", FILE_NAME);
            break;
        }
    }

    delete [] buffer;
    delete openFile;
}

#endif





void
PerformanceTest()
{   

#ifdef TEST_EJ1
  if (!fileSystem->Create(FILE_NAME, FILE_SIZE)) {
    fprintf(stderr, "Perf test: cannot create %s\n", FILE_NAME);
    return;
  }
  int n = NUMBEROFTHREADS;
  Thread **threads = new Thread * [n];
  char ** arg = new char * [NUMBEROFTHREADS];
  for(int i=0; i<n; i++){
    char * c = new char [64];
    strncpy(c, std::to_string(i).c_str(), 64);
    threads[i] = new Thread(c);
    arg[i] = new char[2];
    arg[i][1] = 0;
    arg[i][0] = 'A' + i;
    threads[i]->Fork(simpleThread, (void *) arg[i]);
  }


#else
#ifdef TEST_DIRECTORIES

    string l1 = "/";
    string l2 = "/level1/";
    fileSystem->Mkdir(l1,"level1");
    fileSystem->Mkdir(l2,"level2");
    fileSystem->Mkdir(l2,"level2.1");
    fileSystem->Create("sort",1111,l1);
    fileSystem->Create("sort",1111,l2);
    
  
#else 
    printf("Starting file system performance test:\n");
    stats->Print();
    FileWrite();
    FileRead();
    if (!fileSystem->Remove(FILE_NAME)) {
        printf("Perf test: unable to remove %s\n", FILE_NAME);
        return;
    }
    stats->Print();
#endif
#endif
}
