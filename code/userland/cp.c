/// Simple program to test whether running a user program works.
///
///
/// Just do a “syscall” that shuts down the OS.
/// NOTE: for some reason, user programs with global data structures
/// sometimes have not worked in the Nachos environment.  So be careful out
/// there!  One option is to allocate data structures as automatics within a
/// procedure, but if you do this, you have to be careful to allocate a big
/// enough stack to hold the automatics!


#include "syscall.h"


int
main(int argc, char ** argv)
{

    char buffer[1];
    int n;
    
    Create(argv[2]);
    OpenFileId source = Open(argv[1]);
    OpenFileId destiny = Open(argv[2]);
        
    n = Read(buffer, 1, source);
    while (n > 0){
        Write(buffer, 1, destiny);
        n = Read(buffer, 1, source);
    }

    Close(source);
    Close(destiny);
 
    return 0;
}