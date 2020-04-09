/// Simple test case for the threads assignment.
///
/// Create several threads, and have them context switch back and forth
/// between themselves by calling `Thread::Yield`, to illustrate the inner
/// workings of the thread system.
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2007-2009 Universidad de Las Palmas de Gran Canaria.
///               2016-2017 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "system.hh"
#include "synch.hh"
#include <stdio.h>
#include <string.h>
// #define SEMAPHORE_TEST
// #define LOCK_TEST

Lock *cLock = new Lock("cLock");
Condition *condition = new Condition("Condition",cLock);

int shared = 1;
/// Loop 10 times, yielding the CPU to another ready thread each iteration.
///
/// * `name` points to a string with a thread name, just for debugging
///   purposes.
void
Suma(void *name_)
{

    char *name = (char *) name_;
    
    for (unsigned num = 0; num < 10; num++) {
        
        cLock->Acquire();
        

        shared++;
        printf("*** Thread `%s` is running: iteration %u shared val: %i\n", name, num,shared);

        cLock->Release();
        if(shared == 1){
            condition->Broadcast();
        }

        
        currentThread->Yield();
    }

    printf("!!! Thread `%s` has finished\n", name);
    
}
void
Resta(void *name_)
{

    char *name = (char *) name_;
    
    for (unsigned num = 0; num < 10; num++) {
        
        cLock->Acquire();
        
        
        if(shared == 0)condition->Wait();

        shared--;
        printf("*** Thread `%s` is running: iteration %u shared val: %i\n", name, num,shared);

        cLock->Release();
        currentThread->Yield();
    }

    printf("!!! Thread `%s` has finished\n", name);
    
}
/// Set up a ping-pong between several threads.
///
/// Do it by launching ten threads which call `SimpleThread`, and finally
/// calling `SimpleThread` ourselves.
void
ThreadTest()
{
    DEBUG('t', "Entering thread test\n");
    
    char **names = new char * [4];
    for(int i=0; i<5; i++){
    	names[i] = new char[64];
    }

    strncpy(names[0], "2nd", 64);
    strncpy(names[1], "3rd", 64);
    strncpy(names[2], "4th", 64);
    strncpy(names[3], "5th", 64);

    Thread **threads = new Thread * [5];

    // int i=0;
    // for(; i<2; i++){
    // 	threads[i] = new Thread(names[i]);
    // 	threads[i]->Fork(SimpleThread, (void *) names[i]);
    // }

    threads[0] = new Thread(names[0]);
    threads[0]->Fork(Suma,(void *) names[0]);
    threads[1] = new Thread(names[1]);
    threads[1]->Fork(Suma,(void *) names[1]);

    threads[2] = new Thread(names[2]);
    threads[2]->Fork(Resta,(void *) names[2]);
    Resta((void *) "1st");
}
