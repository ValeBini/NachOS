/// Routines for managing the disk file header (in UNIX, this would be called
/// the i-node).
///
/// The file header is used to locate where on disk the file's data is
/// stored.  We implement this as a fixed size table of pointers -- each
/// entry in the table points to the disk sector containing that portion of
/// the file data (in other words, there are no indirect or doubly indirect
/// blocks). The table size is chosen so that the file header will be just
/// big enough to fit in one disk sector,
///
/// Unlike in a real system, we do not keep track of file permissions,
/// ownership, last modification date, etc., in the file header.
///
/// A file header can be initialized in two ways:
///
/// * for a new file, by modifying the in-memory data structure to point to
///   the newly allocated data blocks;
/// * for a file already on disk, by reading the file header from disk.
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2016-2020 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "file_header.hh"
#include "threads/system.hh"

#include <ctype.h>
#include <stdio.h>





#ifdef BIG_MAX_SIZE
unsigned NeededSectors(unsigned numSectors){
    if(numSectors <= NUM_DIRECT){
        return numSectors;
    }
    return numSectors + 1 + DivRoundUp(numSectors - NUM_DIRECT , TABLE_SIZE);
}
#endif

/// Initialize a fresh file header for a newly created file.  Allocate data
/// blocks for the file out of the map of free disk blocks.  Return false if
/// there are not enough free blocks to accomodate the new file.
///
/// * `freeMap` is the bit map of free disk sectors.
/// * `fileSize` is the bit map of free disk sectors.
bool
FileHeader::Allocate(Bitmap *freeMap, unsigned fileSize)
{
    ASSERT(freeMap != nullptr);

    if (fileSize > MAX_FILE_SIZE)
        return false;

    raw.numBytes = fileSize;
    raw.numSectors = DivRoundUp(fileSize, SECTOR_SIZE);

#ifdef BIG_MAX_SIZE
    unsigned totalSectors = NeededSectors(raw.numSectors);     // TABLE + FILEDATA
    unsigned tableSectors = totalSectors - raw.numSectors - 1; // TABLE
    unsigned dataLeftSectors = raw.numSectors;                    // FILEDATA Left

    if (freeMap->CountClear() < totalSectors)
        return false;  // Not enough space.
    
    for (unsigned i = 0; i < minn(totalSectors,NUM_DIRECT); i++) // Allocate Direct
        raw.dataSectors[i] = freeMap->Find();

    if(totalSectors <= NUM_DIRECT) 
        return true;

    dataLeftSectors -= NUM_DIRECT; 

    raw.table = freeMap->Find();
    unsigned firstIndSector [TABLE_SIZE];
    
    for (unsigned i = 0; i < tableSectors; i++) // Allocate First Indirect
        firstIndSector[i] = freeMap->Find();
        
    synchDisk->WriteSector(raw.table, (char *) firstIndSector);


    for (unsigned i = 0; i < tableSectors; i++){ 
        unsigned secondIndSectors[TABLE_SIZE];
        unsigned j = 0;
        unsigned limite = minn(TABLE_SIZE,dataLeftSectors);
        for(;j<limite;j++,dataLeftSectors--){
            secondIndSectors[j] = freeMap->Find();
        }
        
        synchDisk->WriteSector(firstIndSector[i], (char *) secondIndSectors);
        
    }

    return true;

#else

    if (freeMap->CountClear() < raw.numSectors)
        return false;  // Not enough space.

    for (unsigned i = 0; i < raw.numSectors; i++)
        raw.dataSectors[i] = freeMap->Find();
    return true;

#endif
}

/// De-allocate all the space allocated for data blocks for this file.
///
/// * `freeMap` is the bit map of free disk sectors.
void
FileHeader::Deallocate(Bitmap *freeMap)
{
    ASSERT(freeMap != nullptr);

#ifdef BIG_MAX_SIZE
    
    unsigned totalSectors = NeededSectors(raw.numSectors);       // TABLE + FILEDATA
    unsigned tableSectors = totalSectors - raw.numSectors - 1;      // TABLE
    unsigned dataLeftSectors = raw.numSectors;                      // FILEDATA Left
    
    for (unsigned i = 0; i < minn(totalSectors,NUM_DIRECT); i++){// Deallocate Directs 
        ASSERT(freeMap->Test(raw.dataSectors[i]));  // ought to be marked!
        freeMap->Clear(raw.dataSectors[i]);
    }

    dataLeftSectors -= NUM_DIRECT;
    
    if(totalSectors <= NUM_DIRECT) return;

    //char firstIndSector [SECTOR_SIZE];
    unsigned firstIndSector [TABLE_SIZE];

    synchDisk->ReadSector(raw.table, (char *) firstIndSector);

    for (unsigned i = 0; i < tableSectors; i++){ 
        unsigned secondIndSectors[TABLE_SIZE];
        synchDisk->ReadSector(firstIndSector[i], (char *) secondIndSectors);

        unsigned j = 0;
        unsigned limite = minn(TABLE_SIZE,dataLeftSectors);
        for(;j<limite;j++,dataLeftSectors--){
            ASSERT(freeMap->Test(secondIndSectors[j]));  // ought to be marked!
            freeMap->Clear(secondIndSectors[j]);
        }
        
        ASSERT(freeMap->Test(firstIndSector[i]));  // ought to be marked!
        freeMap->Clear(firstIndSector[i]);
        
    }

    ASSERT(freeMap->Test(raw.table));  // ought to be marked!
    freeMap->Clear(raw.table);

#else
    for (unsigned i = 0; i < raw.numSectors; i++) {
        ASSERT(freeMap->Test(raw.dataSectors[i]));  // ought to be marked!
        freeMap->Clear(raw.dataSectors[i]);
    }
#endif

}

/// Fetch contents of file header from disk.
///
/// * `sector` is the disk sector containing the file header.
void
FileHeader::FetchFrom(unsigned sector)
{
    synchDisk->ReadSector(sector, (char *) &raw);
}

/// Write the modified contents of the file header back to disk.
///
/// * `sector` is the disk sector to contain the file header.
void
FileHeader::WriteBack(unsigned sector)
{
    synchDisk->WriteSector(sector, (char *) &raw);
}

/// Return which disk sector is storing a particular byte within the file.
/// This is essentially a translation from a virtual address (the offset in
/// the file) to a physical address (the sector where the data at the offset
/// is stored).
///
/// * `offset` is the location within the file of the byte in question.
unsigned
FileHeader::ByteToSector(unsigned offset)
{
#ifdef BIG_MAX_SIZE
    unsigned totalSectors = NeededSectors(raw.numSectors);       // TABLE + FILEDATA
    unsigned tableSectors = totalSectors - raw.numSectors - 1;      // TABLE
    unsigned dataLeftSectors = raw.numSectors;                      // FILEDATA Left
    
    if((offset / SECTOR_SIZE) < NUM_DIRECT)                      //Case Direct
        return raw.dataSectors[offset / SECTOR_SIZE];

    unsigned firstIndSector [TABLE_SIZE];

    synchDisk->ReadSector(raw.table, (char *) firstIndSector);

    unsigned firstIndPos = (offset - (SECTOR_SIZE * NUM_DIRECT)) / (TABLE_SIZE * SECTOR_SIZE);

    unsigned secondIndSector [TABLE_SIZE];

    synchDisk->ReadSector(firstIndSector[firstIndPos], (char *) secondIndSector);

    unsigned secondIndPos = ((offset - (SECTOR_SIZE * NUM_DIRECT)) % (TABLE_SIZE * SECTOR_SIZE) / SECTOR_SIZE);
    
    return secondIndSector[secondIndPos];
#else
    return raw.dataSectors[offset / SECTOR_SIZE];
#endif
}

/// Return the number of bytes in the file.
unsigned
FileHeader::FileLength() const
{
    return raw.numBytes;
}

/// Print the contents of the file header, and the contents of all the data
/// blocks pointed to by the file header.
void
FileHeader::Print(const char *title)
{

#ifndef BIG_MAX_SIZE
    char *data = new char [SECTOR_SIZE];

    if (title == nullptr)
        printf("File header:\n");
    else
        printf("%s file header:\n", title);

    printf("    size: %u bytes\n"
           "    block indexes: ",
           raw.numBytes);

    for (unsigned i = 0; i < minn(NUM_DIRECT,raw.numSectors); i++)
        printf("%u ", raw.dataSectors[i]);
    printf("\n");

    for (unsigned i = 0, k = 0; i < minn(NUM_DIRECT,raw.numSectors); i++) {
        printf("    contents of block %u:\n", raw.dataSectors[i]);
        synchDisk->ReadSector(raw.dataSectors[i], data);
        for (unsigned j = 0; j < SECTOR_SIZE && k < raw.numBytes; j++, k++) {
            if (isprint(data[j]))
                printf("%c", data[j]);
            else
                printf("\\%X", (unsigned char) data[j]);
        }
        printf("\n");
    }
    delete [] data;

#else

    unsigned totalSectors = NeededSectors(raw.numSectors);       // TABLE + FILEDATA
    unsigned tableSectors = totalSectors - raw.numSectors - 1;      // TABLE
    unsigned dataLeftSectors = raw.numSectors;                      // FILEDATA Left
    
    char *data = new char [SECTOR_SIZE];

    if (title == nullptr)
        printf("File header:\n");
    else
        printf("%s file header:\n", title);

    printf("    size: %u bytes\n", raw.numBytes);

    unsigned sec = 0;
    unsigned contentSectors [dataLeftSectors];

    // Direct
    if(totalSectors <= NUM_DIRECT){
        printf("Only direct blocks: \n");
        for (unsigned i = 0; i < totalSectors; i++){
                contentSectors[sec] = raw.dataSectors[i];
                sec++;
                printf("%u ", raw.dataSectors[i]);
            }
        printf("\n");
    } else {

    for (unsigned i = 0; i < NUM_DIRECT; i++, sec++)
        contentSectors[sec] = raw.dataSectors[i];
    
    // First Table
    unsigned firstIndSector [TABLE_SIZE];
    synchDisk->ReadSector(raw.table, (char *) firstIndSector);

    printf("First indirection table blocks: \n");
    for (unsigned i = 0; i < tableSectors ; i++)
        printf("%u ", firstIndSector[i]);
    printf("\n");

    // Second Tables

    for (unsigned i = 0; i < tableSectors; i++){
        unsigned secondIndSector [TABLE_SIZE];
        synchDisk->ReadSector(firstIndSector[i], (char *) secondIndSector);
        
        printf("Second indirection table %u blocks: \n", i);
        
        
        unsigned limite;
        if(i == tableSectors-1) limite = (dataLeftSectors-NUM_DIRECT) % TABLE_SIZE;
        else limite = TABLE_SIZE;

        for (unsigned j = 0; j < limite ; j++) {
            printf("%u ", secondIndSector[j]);
            contentSectors[sec] = secondIndSector[j];
            sec++;
        }
        printf("\n");
        
    }

    }


    for (unsigned i = 0, k = 0; i < sec; i++) {
        printf("    contents of block %u:\n", contentSectors[i]);
        
        synchDisk->ReadSector(contentSectors[i], data);
        for (unsigned j = 0; j < SECTOR_SIZE && k < raw.numBytes; j++, k++) {
            if (isprint(data[j]))
                printf("%c", data[j]);
            else
                printf("\\%X", (unsigned char) data[j]);
        }
        printf("\n");
    }
    delete [] data;
 
#endif
}

const RawFileHeader *
FileHeader::GetRaw() const
{
    return &raw;
}
