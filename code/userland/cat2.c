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
    OpenFileId o;
    int n;
    // for(int i = 0 ; i < argc ; i++ )Write(argv[i],12,CONSOLE_OUTPUT);
    
    for (int i = 1; i < argc; i++) {
        o = Open(argv[i]);
        
        n = Read(buffer,1,o);
        while (n > 0){
            Write(buffer,1,CONSOLE_OUTPUT);
            n = Read(buffer,1,o);
        }

        Close(o);
    }

    return 0;
 
}
