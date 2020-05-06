/// Copyright (c) 2019-2020 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "transfer.hh"
#include "lib/utility.hh"
#include "threads/system.hh"


void ReadBufferFromUser(int userAddress, char *outBuffer,
                        unsigned byteCount)
{
    ASSERT(userAddress != 0);
    ASSERT(outBuffer != nullptr);
    ASSERT(byteCount != 0);

    unsigned count = 0;
    do {
        int temp;
        count++;
        int i;
        for(i = 0; i<MAX_PAGE_FAULTS && !(machine->ReadMem(userAddress++, 1, &temp)); i++);
        ASSERT(i<MAX_PAGE_FAULTS);
        *outBuffer++ = (unsigned char) temp;
    } while (count < byteCount);

    

}


bool ReadStringFromUser(int userAddress, char *outString,
                        unsigned maxByteCount)
{
    ASSERT(userAddress != 0);
    ASSERT(outString != nullptr);
    ASSERT(maxByteCount != 0);

    unsigned count = 0;
    do {
        int temp;
        count++;
        int i;
        for(i = 0; i<MAX_PAGE_FAULTS && !(machine->ReadMem(userAddress++, 1, &temp)); i++);
        ASSERT(i<MAX_PAGE_FAULTS);
        *outString = (unsigned char) temp;
    } while (*outString++ != '\0' && count < maxByteCount);

    return *(outString - 1) == '\0';
}

void WriteBufferToUser(const char *buffer, int userAddress,
                       unsigned byteCount)
{
    ASSERT(userAddress != 0);
    ASSERT(buffer != nullptr);
    ASSERT(byteCount != 0);

    unsigned count = 0;
    do {
        int i;
        for(i = 0; i<MAX_PAGE_FAULTS && !(machine->WriteMem(userAddress++, 1, buffer[count])); i++);
        ASSERT(i<MAX_PAGE_FAULTS);
        count++;
        // *outBuffer = (unsigned char) temp;
    } while (count < byteCount);
}

void WriteStringToUser(const char *string, int userAddress)
{

    ASSERT(userAddress != 0);
    ASSERT(string != nullptr);

    unsigned count = 0;
    do {
        int i;
        for(i = 0; i<MAX_PAGE_FAULTS && !(machine->WriteMem(userAddress++, 1, string[count])); i++);
        ASSERT(i<MAX_PAGE_FAULTS);
        count++;
    } while ((string[count]) != '\0' );

}
