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
// #include "thread.hh"
#include <stdio.h>
#include <string.h>
#include <string>
// #define SEMAPHORE_TEST
#define LOCK_TEST
// #define CONDITION_TEST_1
// #define CONDITION_TEST_2
// #define CHANNEL_TEST
// #define JOIN_TEST

#ifdef SEMAPHORE_TEST
Semaphore *sem = new Semaphore("Semaforo", 3);
#endif

#ifdef LOCK_TEST
Lock *lock = new Lock("Lock");
#endif

#if defined(CONDITION_TEST_1) || defined(CONDITION_TEST_2)
Lock *cLock = new Lock("cLock");
Condition *condition = new Condition("Condition",cLock);
#endif

#ifdef CHANNEL_TEST
Channel canal("Panama");
#endif

/// Loop 10 times, yielding the CPU to another ready thread each iteration.
///
/// * `name` points to a string with a thread name, just for debugging
///   purposes.
void
SimpleThread(void *name_)
{
    #ifdef SEMAPHORE_TEST
    sem->P();
    #endif

    #ifdef LOCK_TEST
    lock->Acquire();
    #endif



    // Reinterpret arg `name` as a string.
    char *name = (char *) name_;

    // If the lines dealing with interrupts are commented, the code will
    // behave incorrectly, because printf execution may cause race
    // conditions.

    for (unsigned num = 0; num < 10; num++) {
        printf("*** Thread `%s` is running: iteration %u\n", name, num);
        currentThread->Yield();
    }

    printf("!!! Thread `%s` has finished\n", name);

    #ifdef SEMAPHORE_TEST
    sem->V();
    #endif

    #ifdef LOCK_TEST
    lock->Release();
    #endif


}


#if defined(CONDITION_TEST_1) || defined(CONDITION_TEST_2)

void
Add(void *x){
  int * shared = (int *) x;
  while(1){
    cLock->Acquire();
    (*shared)++;
    printf("*** ADD : Thread `%s` is running %d\n", currentThread->GetName(),*shared);
    if((*shared)>0) condition->Signal();
    cLock->Release();
    currentThread->Yield();
  }
}

void
Substract(void *x){
  int * shared = (int *) x;
  while(1){
    cLock->Acquire();
    while((*shared)<=0) condition->Wait();
    (*shared)--;
    printf("*** SUB : Thread `%s` is running %d\n", currentThread->GetName(),*shared);
    cLock->Release();
    currentThread->Yield();
  }
}



void
PrintId(void *x){
  cLock->Acquire();
  int * ready = (int *) x;
  while (!(*ready)) condition->Wait();
  cLock->Release();
  printf("*** Thread `%s` is running %d\n", currentThread->GetName(),*ready);
  currentThread->Yield();
}


void
Go(void *x){
  cLock->Acquire();
  int * ready = (int *) x;
  *ready = 1;
  cLock->Release();
  condition->Broadcast();
}

#endif


#ifdef CHANNEL_TEST
void
SimpleThreadSend(void *name_)
{
    int msg = -1,msg2 = -2,msg3 = -3;
    canal.Send(msg);
    canal.Send(msg2);
    canal.Send(msg3);
}
void
SimpleThreadReceive(void *name_)
{
  int valor = 1,valor2 = 2,valor3 = 3;
  canal.Receive(&valor);
  canal.Receive(&valor2);
  canal.Receive(&valor3);
  printf("El valor es %d\n",valor);
  printf("El valor es %d\n",valor2);
  printf("El valor es %d\n",valor3);
}
#endif

/// Set up a ping-pong between several threads.
///
/// Do it by launching ten threads which call `SimpleThread`, and finally
/// calling `SimpleThread` ourselves.
void
ThreadTest()
{
    DEBUG('t', "Entering thread test\n");

#if defined(SEMAPHORE_TEST) || defined(LOCK_TEST)

    char **names = new char * [4];
    for(int i=0; i<4; i++){
    	names[i] = new char[64];
    }

    strncpy(names[0], "2nd", 64);
    strncpy(names[1], "3rd", 64);
    strncpy(names[2], "4th", 64);
    strncpy(names[3], "5th", 64);

    Thread **threads = new Thread * [4];

    for(int i=0; i<4; i++){
    	threads[i] = new Thread(names[i]);
      threads[i]->SetOriginalPriority((unsigned int) i);
    	threads[i]->Fork(SimpleThread, (void *) names[i]);
    }

    SimpleThread((void *) "1st");

#endif

#ifdef CONDITION_TEST_1

    int n = 109;
    int * shared = new int;
    *shared = 0;
    Thread **threads = new Thread * [n];

    for(int i=0; i<n; i++){
      char * c = new char [64];
      strncpy(c, std::to_string(i).c_str(), 64);
      threads[i] = new Thread(c);
    	threads[i]->Fork(PrintId, (void *) shared);
    }

    Go(shared);

#endif

#ifdef CONDITION_TEST_2

  int n = 10;
  int * shared = new int;
  *shared = 5;
  Thread **threads = new Thread * [n];

  for(int i=0; i<n; i++){
    char * c = new char [64];
    strncpy(c, std::to_string(i).c_str(), 64);
    threads[i] = new Thread(c);
    if(i*2>n)
      threads[i]->Fork(Add, (void *) shared);
    else
      threads[i]->Fork(Substract, (void *) shared);
  }

#endif

#ifdef CHANNEL_TEST

    char *name = new char [64];
    strncpy(name, "2nd", 64);
    Thread *newThread = new Thread(name);
    newThread->Fork(SimpleThreadSend, (void *) name);
    SimpleThreadReceive((void *) "1st");

#endif

#ifdef JOIN_TEST

printf("INicio padre\n");
char *name = new char [64];
strncpy(name, "2nd", 64);
Thread *t = new Thread("Hijo",true);
t->Fork(SimpleThread, (void *) name);
t->Join(); // Ac´a el hilo en ejecuci´on se bloquea
// hasta que ‘t’ termine.
printf("Fin padre\n");

#endif


}
