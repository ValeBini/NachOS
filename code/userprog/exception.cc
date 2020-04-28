/// Entry points into the Nachos kernel from user programs.
///
/// There are two kinds of things that can cause control to transfer back to
/// here from user code:
///
/// * System calls: the user code explicitly requests to call a procedure in
///   the Nachos kernel.  Right now, the only function we support is `Halt`.
///
/// * Exceptions: the user code does something that the CPU cannot handle.
///   For instance, accessing memory that does not exist, arithmetic errors,
///   etc.
///
/// Interrupts (which can also cause control to transfer from user code into
/// the Nachos kernel) are handled elsewhere.
///
/// For now, this only handles the `Halt` system call.  Everything else core-
/// dumps.
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2016-2020 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "transfer.hh"
#include "syscall.h"
#include "filesys/directory_entry.hh"
#include "threads/system.hh"

#include <stdio.h>

#include <string.h>


static void
IncrementPC()
{
    unsigned pc;

    pc = machine->ReadRegister(PC_REG);
    machine->WriteRegister(PREV_PC_REG, pc);
    pc = machine->ReadRegister(NEXT_PC_REG);
    machine->WriteRegister(PC_REG, pc);
    pc += 4;
    machine->WriteRegister(NEXT_PC_REG, pc);
}

/// Do some default behavior for an unexpected exception.
///
/// NOTE: this function is meant specifically for unexpected exceptions.  If
/// you implement a new behavior for some exception, do not extend this
/// function: assign a new handler instead.
///
/// * `et` is the kind of exception.  The list of possible exceptions is in
///   `machine/exception_type.hh`.
static void
DefaultHandler(ExceptionType et)
{
    int exceptionArg = machine->ReadRegister(2);

    fprintf(stderr, "Unexpected user mode exception: %s, arg %d.\n",
            ExceptionTypeToString(et), exceptionArg);
    ASSERT(false);
}

/// Handle a system call exception.
///
/// * `et` is the kind of exception.  The list of possible exceptions is in
///   `machine/exception_type.hh`.
///
/// The calling convention is the following:
///
/// * system call identifier in `r2`;
/// * 1st argument in `r4`;
/// * 2nd argument in `r5`;
/// * 3rd argument in `r6`;
/// * 4th argument in `r7`;
/// * the result of the system call, if any, must be put back into `r2`.
///
/// And do not forget to increment the program counter before returning. (Or
/// else you will loop making the same system call forever!)
static void
SyscallHandler(ExceptionType _et)
{
    int scid = machine->ReadRegister(2);

    switch (scid) {

        case SC_HALT:
            DEBUG('e', "Shutdown, initiated by user program.\n");
            interrupt->Halt();
            break;

        case SC_CREATE: {
            int filenameAddr = machine->ReadRegister(4);
            if (filenameAddr == 0) {
                DEBUG('e', "Error: address to filename string is null.\n");
                machine->WriteRegister(2, -1);
                break;
            }
                

            char filename[FILE_NAME_MAX_LEN + 1];
            if (!ReadStringFromUser(filenameAddr, filename, sizeof filename)) {
                DEBUG('e', "Error: filename string too long (maximum is %u bytes).\n",
                      FILE_NAME_MAX_LEN);
                machine->WriteRegister(2, -1);
                break;
            }

            DEBUG('e', "`Create` requested for file `%s`.\n", filename);
            
            ASSERT(fileSystem->Create(filename,1000));

            break;
        }

        case SC_OPEN: {
            int filenameAddr = machine->ReadRegister(4);
            if (filenameAddr == 0){
                DEBUG('e', "Error: address to filename string is null.\n");
                machine->WriteRegister(2, -1);
                break;
            }

            char filename[FILE_NAME_MAX_LEN + 1];
            if (!ReadStringFromUser(filenameAddr, filename, sizeof filename)) {
                DEBUG('e', "Error: filename string too long (maximum is %u bytes).\n",
                      FILE_NAME_MAX_LEN);
                machine->WriteRegister(2, -1);
                break;
            }

            DEBUG('e', "`Open` requested for file `%s`.\n", filename);
            
            OpenFile *openFile = fileSystem->Open(filename);
            ASSERT(openFile!=nullptr);
            int index = currentThread->openFiles->Add(openFile);
            
            if(index == -1) {
                DEBUG('e', "Error: opened files' table is full.\n");
                machine->WriteRegister(2, -1);
                delete openFile;
                break;
            }

            machine->WriteRegister(2,index);

            DEBUG('e', "`Open` finished for file `%s`.\n", filename);

            break;
        }

        case SC_CLOSE: {
            int fid = machine->ReadRegister(4);
            DEBUG('e', "`Close` requested for id %u.\n", fid);

            if(!currentThread->openFiles->HasKey(fid)){
                DEBUG('e', "Error: openFileId doesn't exist.\n");
                machine->WriteRegister(2, -1);
                break;
            }

            OpenFile *openFile = currentThread->openFiles->Get(fid);
            currentThread->openFiles->Remove(fid);
            delete openFile;

            DEBUG('e', "`Close` finished for id `%u`.\n", fid);
            machine->WriteRegister(2, 0);
            break;
        }

        case SC_WRITE: {
            
            int bufferAddr = machine->ReadRegister(4);
            int size = machine->ReadRegister(5);
            OpenFileId openFileId = machine->ReadRegister(6);

            if (bufferAddr == 0) {
                DEBUG('e', "Error: address to buffer is null.\n");
                machine->WriteRegister(2, -1);
                break;
            }
            
            char buffer[size+1];
            ReadBufferFromUser(bufferAddr, buffer, size);
            buffer[size] = 0;

            DEBUG('e', "`Write` requested in userAddr %d buffer %s , with size %d. And openfileId %d \n",bufferAddr,buffer,size,openFileId);
           
            if(openFileId == CONSOLE_OUTPUT) {
                for (int i = 0; i < size; i++) {
                    synchConsole->PutChar(buffer[i]);
                }
            }
            else {
                if(!currentThread->openFiles->HasKey(openFileId)){
                    DEBUG('e', "Error: openFileId doesn't exist.\n");
                    machine->WriteRegister(2, -1);
                    break;
                }
            
                // for (int i=0; i<size; i++)
                //     DEBUG('e', "%d\n", (int)buffer[i]);

                OpenFile *file = currentThread->openFiles->Get(openFileId);
                file->Write(buffer,size);
            }
            
            machine->WriteRegister(2, size);
            break;
        }

        case SC_READ:{

            int bufferAddr = machine->ReadRegister(4);
            int size = machine->ReadRegister(5);
            OpenFileId openFileId = machine->ReadRegister(6);
 
            DEBUG('e', "`Read` requested in userAddr %d buffer from openfileId %d \n",bufferAddr,openFileId);

            if (bufferAddr == 0) {
                DEBUG('e', "Error: address to buffer is null.\n");
                machine->WriteRegister(2, -1);
                break;
            }
      
            char buffer[size+1];

            if(openFileId == CONSOLE_INPUT) {
                int i;
                for(i = 0; i < size; i++) {
                    buffer[i] = synchConsole->GetChar();
                    if(buffer[i] == '\n') 
                        break;
                }
                buffer[i] = 0;
                machine->WriteRegister(2, i);
            }
            else {
                if(!currentThread->openFiles->HasKey(openFileId)){
                    DEBUG('e', "Error: openFileId doesn't exist.\n");
                    machine->WriteRegister(2, -1);
                    break;
                }

                OpenFile *file = currentThread->openFiles->Get(openFileId);
                int n = file->Read(buffer,size);     

                buffer[size] = 0;

                // for (int i=0; i<size; i++)
                //     DEBUG('e', "%d\n", (int)buffer[i]);
                
                machine->WriteRegister(2, n);
            }
            
            WriteStringToUser(buffer, bufferAddr);

            DEBUG('e', "`Read` finished in userAddr %d buffer from openfileId %d. Read: %s Size: %u \n",bufferAddr,openFileId,buffer,strlen(buffer));

            break;
        }

        default:
            fprintf(stderr, "Unexpected system call: id %d.\n", scid);
            ASSERT(false);

    }

    IncrementPC();
}


/// By default, only system calls have their own handler.  All other
/// exception types are assigned the default handler.
void
SetExceptionHandlers()
{
    machine->SetHandler(NO_EXCEPTION,            &DefaultHandler);
    machine->SetHandler(SYSCALL_EXCEPTION,       &SyscallHandler);
    machine->SetHandler(PAGE_FAULT_EXCEPTION,    &DefaultHandler);
    machine->SetHandler(READ_ONLY_EXCEPTION,     &DefaultHandler);
    machine->SetHandler(BUS_ERROR_EXCEPTION,     &DefaultHandler);
    machine->SetHandler(ADDRESS_ERROR_EXCEPTION, &DefaultHandler);
    machine->SetHandler(OVERFLOW_EXCEPTION,      &DefaultHandler);
    machine->SetHandler(ILLEGAL_INSTR_EXCEPTION, &DefaultHandler);
}
