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
//#include <string.h>
//#include <stdio.h>

int
main(int argc, char ** argv)
{

    char buffer[12];

    OpenFileId o = Open("test.txt");
    Read(buffer,12,o);
    Close(o);

    Write(buffer,12,CONSOLE_OUTPUT);

    
    // for(int i = 0 ; i<argc; i++){
    //     // char buffer[10];
    //     // strcpy(buffer, argv[i]);
    //     //fprintf(stderr, "Arg: %d, arg %s.\n",i,argv[i]);
    //     Write(argv[i],3,CONSOLE_OUTPUT);
    //     // Write("123456789",9,CONSOLE_OUTPUT);
    // }

    return 0;
 
}
