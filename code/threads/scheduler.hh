/// Data structures for the thread dispatcher and scheduler.
///
/// Primarily, the list of threads that are ready to run.
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2016-2020 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.

#ifndef NACHOS_THREADS_SCHEDULER__HH
#define NACHOS_THREADS_SCHEDULER__HH

#define N_QUEUES 10

#include "thread.hh"
#include "lib/list.hh"
class MultiQueue {
   public:

    MultiQueue();

    ~MultiQueue();

    void Push(unsigned int p, Thread* t);

    Thread* Pop();

    bool isEmpty();

    bool Has(Thread * t);

    void RaisePriority(Thread * t, unsigned int p);

    void ApplyToQueue(void (*func)(Thread *), int i);

   private:

    List<Thread*>* queues[N_QUEUES];

};


/// The following class defines the scheduler/dispatcher abstraction --
/// the data structures and operations needed to keep track of which
/// thread is running, and which threads are ready but not running.
class Scheduler {
public:

    /// Initialize list of ready threads.
    Scheduler();

    /// De-allocate ready list.
    ~Scheduler();

    /// Thread can be dispatched.
    void ReadyToRun(Thread *thread);

    /// Dequeue first thread on the ready list, if any, and return thread.
    Thread *FindNextToRun();

    /// Cause `nextThread` to start running.
    void Run(Thread *nextThread);

    // Print contents of ready list.
    void Print();

    void RaisePriority(Thread * t, unsigned int p);

    bool IsThreadInReadyList(Thread * t);

    void AddToFinished(Thread * t);

    bool IsFinished(Thread * t);

private:

    MultiQueue* readyList;

    List <Thread *> *finishedThreads;
};



#endif
