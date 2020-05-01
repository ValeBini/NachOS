#include "syscall.h"

#include <malloc.h>
#include <string.h>


int
main(void)
{
    SpaceId    newProc;
    OpenFileId input  = CONSOLE_INPUT;
    OpenFileId output = CONSOLE_OUTPUT;
    char       prompt[2] = { '-', '-' };
    char       ch, buffer[60];
    int        i;

    for (;;) {
        Write(prompt, 2, output);
        i = 0;
        do
            Read(&buffer[i], 1, input);
        while (buffer[i++] != '\n' );

        buffer[--i] = '\0';
        
        char *argv [2];
        argv[0] = malloc(sizeof(char)*10);
        argv[1] = malloc(sizeof(char)*10);
        strcpy(argv[0],"hola");
        strcpy(argv[1],"chau");
        if (i > 0) {
            newProc = Exec(buffer, argv, 1);
            Join(newProc);
        }
    }

    return -1;
}
