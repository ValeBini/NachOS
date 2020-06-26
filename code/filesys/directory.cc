/// Routines to manage a directory of file names.
///
/// The directory is a table of fixed length entries; each entry represents a
/// single file, and contains the file name, and the location of the file
/// header on disk.  The fixed size of each directory entry means that we
/// have the restriction of a fixed maximum size for file names.
///
/// The constructor initializes an empty directory of a certain size; we use
/// ReadFrom/WriteBack to fetch the contents of the directory from disk, and
/// to write back any modifications back to disk.
///
/// Also, this implementation has the restriction that the size of the
/// directory cannot expand.  In other words, once all the entries in the
/// directory are used, no more files can be created.  Fixing this is one of
/// the parts to the assignment.
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2016-2020 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "directory.hh"
#include "directory_entry.hh"
#include "file_header.hh"
#include "lib/utility.hh"

#include <stdio.h>
#include <string.h>


/// Initialize a directory; initially, the directory is completely empty.  If
/// the disk is being formatted, an empty directory is all we need, but
/// otherwise, we need to call FetchFrom in order to initialize it from disk.
///
/// * `size` is the number of entries in the directory.
Directory::Directory(unsigned size)
{
    // ASSERT(size > 0);
    raw.table = new DirectoryEntry[1];
    raw.tableSize = 1;
    
    raw.table[0].inUse = false;
    raw.table[0].isDirectory = false;
    
}

/// De-allocate directory data structure.
Directory::~Directory()
{
    delete [] raw.table;
}

/// Read the contents of the directory from disk.
///
/// * `file` is file containing the directory contents.
void
Directory::FetchFrom(OpenFile *file)
{
    ASSERT(file != nullptr);

    if(raw.table != nullptr)
        delete raw.table;

    file->ReadAt((char *) (&raw.tableSize),sizeof (unsigned), 0);

    DEBUG('S',"Size : %d \n",raw.tableSize);

    raw.table = new DirectoryEntry [raw.tableSize];

    for (unsigned i = 0; i < raw.tableSize; i++){
        raw.table[i].inUse = false;
        raw.table[i].isDirectory = false;
    }
    
    file->ReadAt((char *) raw.table, raw.tableSize * sizeof (DirectoryEntry), sizeof (unsigned));
    
}



/// Write any modifications to the directory back to disk.
///
/// * `file` is a file to contain the new directory contents.
void
Directory::WriteBack(OpenFile *file)
{
    ASSERT(file != nullptr);
    DEBUG('S',"Size antes de escribirlo : %d \n",raw.tableSize);
    file->WriteAt((char *) (&raw.tableSize), sizeof(unsigned), 0);
    file->WriteAt((char *) raw.table,
                  raw.tableSize * sizeof (DirectoryEntry), sizeof (unsigned));
}

/// Look up file name in directory, and return its location in the table of
/// directory entries.  Return -1 if the name is not in the directory.
///
/// * `name` is the file name to look up.
int
Directory::FindIndex(const char *name)
{
    ASSERT(name != nullptr);

    for (unsigned i = 0; i < raw.tableSize; i++)
        if (raw.table[i].inUse
              && !strncmp(raw.table[i].name, name, FILE_NAME_MAX_LEN))
            return i;
    return -1;  // name not in directory
}

/// Look up file name in directory, and return the disk sector number where
/// the file's header is stored.  Return -1 if the name is not in the
/// directory.
///
/// * `name` is the file name to look up.
int
Directory::Find(const char *name)
{
    ASSERT(name != nullptr);

    int i = FindIndex(name);
    if (i != -1)
        return raw.table[i].sector;
    return -1;
}

int
Directory::FindDirectory(const char *name)
{
    ASSERT(name != nullptr);

    int i = FindIndex(name);
    if (i != -1 && raw.table[i].isDirectory)
        return raw.table[i].sector;
    return -1;
}


bool
Directory::Add(const char *name, int newSector, bool isDirectory)
{
    ASSERT(name != nullptr);
    DEBUG('R',"Directory add requested. Directory path: %x name: %s\n",this,name);
    if (FindIndex(name) != -1)
        return false;

    for (unsigned i = 0; i < raw.tableSize; i++)
        if (!raw.table[i].inUse) {
            raw.table[i].inUse = true;
            raw.table[i].isDirectory = isDirectory;
            strncpy(raw.table[i].name, name, FILE_NAME_MAX_LEN);
            raw.table[i].sector = newSector;
            DEBUG('R',"Directory add finished.\n");
            return true;
        }

    DEBUG('R',"Directory add need to resize.\n");
    Resize();
    return Add(name,newSector,isDirectory);
}

void
Directory::Resize(){
    DEBUG('R',"Directory resize requested. Directory path: %x\n",this);
    unsigned newSize = raw.tableSize * 2;
    DirectoryEntry * newTable = new DirectoryEntry[newSize];
    memcpy(newTable,raw.table,raw.tableSize * sizeof(DirectoryEntry));
    delete [] raw.table;
    raw.table = newTable;
    for (unsigned i = raw.tableSize; i < newSize; i++){
        raw.table[i].inUse = false;
        raw.table[i].isDirectory = false;
    }
    int oldSize = raw.tableSize;
    raw.tableSize = newSize;

    DEBUG('R',"Directory resize ended. OldSize %d newsize %d\n",oldSize,newSize);
}



/// Remove a file name from the directory.   Return true if successful;
/// return false if the file is not in the directory.
///
/// * `name` is the file name to be removed.
bool
Directory::Remove(const char *name)
{
    ASSERT(name != nullptr);

    int i = FindIndex(name);
    if (i == -1)
        return false;  // name not in directory
    raw.table[i].inUse = false;
    return true;
}

/// List all the file names in the directory.
void
Directory::List() const
{
    for (unsigned i = 0; i < raw.tableSize; i++)
        if (raw.table[i].inUse){
            if(raw.table[i].isDirectory)
            printf("%s  (D)\n", raw.table[i].name);
            if(!raw.table[i].isDirectory)
            printf("%s\n", raw.table[i].name);
            
        }
}


/// List all the file names in the directory, their `FileHeader` locations,
/// and the contents of each file.  For debugging.
void
Directory::Print() const
{
    FileHeader *hdr = new FileHeader;

    printf("Directory contents:\n");
    for (unsigned i = 0; i < raw.tableSize; i++)
        if (raw.table[i].inUse) {
            printf("\nDirectory entry:\n"
                   "    name: %s\n"
                   "    sector: %u\n",
                   raw.table[i].name, raw.table[i].sector);
            hdr->FetchFrom(raw.table[i].sector);
            hdr->Print(nullptr);
        }
    printf("\n");
    delete hdr;
}

const RawDirectory *
Directory::GetRaw() const
{
    return &raw;
}


void
Directory::PrintR(const char * name) const
{
    FileHeader *hdr = new FileHeader;

    printf("\n\n\n ------------------------------ \nDirectory contents of %s:\n",name);
    List();
    for (unsigned i = 0; i < raw.tableSize; i++)
        if (raw.table[i].inUse) {
            // printf("\nDirectory entry:\n"
            //        "    name: %s\n"
            //        "    sector: %u\n",
            //        raw.table[i].name, raw.table[i].sector);
            hdr->FetchFrom(raw.table[i].sector);
            //hdr->Print(nullptr);
            if(raw.table[i].isDirectory){
                Directory *dir = new Directory(NUM_DIR_ENTRIES);
                OpenFile *dirFile = new OpenFile(raw.table[i].sector, raw.table[i].name);
                dir->FetchFrom(dirFile);
                dir->PrintR(raw.table[i].name);
                delete dirFile;
                delete dir;
            }
        }
    //printf("\n ------------------------------ \n Finishing Directory\n\n\n");
    delete hdr;
}

int
Directory::PrintToBuffer(int size,char * buffer){

    string s = "";
    unsigned i;
     for (i = 0; i < raw.tableSize; i++)
        if (raw.table[i].inUse){
            if(raw.table[i].isDirectory)
                s+=string(raw.table[i].name) + "(D)\n"; 
            if(!raw.table[i].isDirectory)
                s+=string(raw.table[i].name) + "\n";
        }

    if((int)(s.size())>size) return -1;

    strcpy(buffer,s.c_str());
    
    if(i==0) return 0;

    return s.size();
}