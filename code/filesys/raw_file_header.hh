/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2016-2020 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.

#ifndef NACHOS_FILESYS_RAWFILEHEADER__HH
#define NACHOS_FILESYS_RAWFILEHEADER__HH

#define BIG_MAX_SIZE



#include "machine/disk.hh"


#ifndef BIG_MAX_FILE

static const unsigned NUM_DIRECT
  = (SECTOR_SIZE - 2 * sizeof (int)) / sizeof (int);
const unsigned MAX_FILE_SIZE = NUM_DIRECT * SECTOR_SIZE;

struct RawFileHeader {
    unsigned numBytes;  ///< Number of bytes in the file.
    unsigned numSectors;  ///< Number of data sectors in the file.
    unsigned dataSectors[NUM_DIRECT];  ///< Disk sector numbers for each data
                                       ///< block in the file.
};

#else

const unsigned TABLE_SIZE = (SECTOR_SIZE/sizeof(int));
static const unsigned NUM_DIRECT = TABLE_SIZE - 3;
const unsigned MAX_FILE_SIZE = NUM_DIRECT * SECTOR_SIZE +  TABLE_SIZE * TABLE_SIZE * SECTOR_SIZE;

struct RawFileHeader {
    unsigned numBytes;  ///< Number of bytes in the file.
    unsigned numSectors;  ///< Number of data sectors in the file.
    unsigned table = 0; /// First Indirect Table
    unsigned dataSectors[NUM_DIRECT];  ///< Disk sector numbers for each data
                                       ///< block in the file.
};
#endif
#endif

