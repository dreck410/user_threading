//threads.c

#include <assert.h>
#include <ucontext.h>
#include <stdio.h>
#include "ut_threads.h"
#include <stdlib.h>
// The following are valid thread statuses:

// This thread is not in use
#define THREAD_UNUSED 0
// This thread has been started and has not finished
#define THREAD_ALIVE 1
// This thread has finished, but no one has called ut_join to obtain its status yet
#define THREAD_ZOMBIE 2

typedef struct {
    ucontext_t context;
    int status;   // one of THREAD_UNUSED, THREAD_ALIVE, THREAD_ZOMBIE
    int exitValue; // thread's return value
} uthread_t;


/*
 * Define the thread table
* Thread ID's are indexes into this table
*/
uthread_t thread[MAX_THREADS]; // the thread table

int curThread; // the index of the currently executing thread

// Do needed initialization work, including setting up stack pointers for all of the threads
int ut_init(char *stackbuf) {
  int i = 0;
    
  // setup stack pointers
  for (i = 0 ; i < MAX_THREADS; ++i ) {
    thread[i].context.uc_stack.ss_sp = stackbuf + i * STACK_SIZE;
    thread[i].context.uc_stack.ss_size = STACK_SIZE;
  }
  
  // initialize main thread
  thread[0].status = THREAD_ALIVE;
  curThread = 0;

  return getcontext(&thread[curThread].context);
}

// Creates a thread with entry point set to <entry>
// <arg> is the argument that will be passed to <entry> in the new thread
// Returns threadID on success, or -1 on failure (thread table full)
int ut_create(void (* entry)(int), int arg)
{
    
    // Find a slot in thread table whose status is THREAD_UNUSED
    int i = 0;
    for (i = 0; i < MAX_THREADS; ++i)
    {
      if (thread[i].status == THREAD_UNUSED)
      { 
        //set the status of the slot to THREAD_ALIVE
        // and initialize its context
        
        // Return the thread Id
        thread[i].status = THREAD_ALIVE;
        
        getcontext(&thread[i].context);

        makecontext(&thread[i].context, entry, 1, arg);
        return i;

      }
    }
    return -1;
    // Return -1 if all slots are in use
    
}

// scheduler - picks a thread to run (possibly this one, if no other threads are available)
void ut_yield()
{
 int i = 0;
  // find a thread that can run, using round robin scheduling; pick this one if no other thread can run
  for (i = (curThread + 1)  % MAX_THREADS; i != curThread; )
  {
    if (thread[i].status == THREAD_ALIVE)
    {
      // if another thread can run, switch to it
      int buf = curThread;
      curThread = i;
      swapcontext(&thread[buf].context, &thread[i].context);
    }
    ++i;
    i = i % MAX_THREADS;
  }
    // if no threads are ALIVE, exit the program
  if (thread[curThread].status == THREAD_ALIVE)
  {
    return;
  }
  
  exit(0);
  
    // otherwise, return to continue running this thread
  
}

// returns thread ID of current thread
int ut_getid()
{
  return curThread;  
}

// Wait for the thread identified by <threadId> to exit, returning its return value in status. 
// In case of success, this function returns 0, otherwise, -1.
int ut_join(int threadId, int *status) 
{
    // return -1 if threadId is illegal (out of range) or if its status is THREAD_UNUSED
    if (threadId >= MAX_THREADS || threadId < 0 || thread[threadId].status == THREAD_UNUSED)
    {
      return -1;
    }

    // busy-wait (calling ut_yield in a loop) while the status of thread <threadId> is THREAD_ALIVE
    while(thread[threadId].status == THREAD_ALIVE){
      ut_yield();
    }

    // If the thread status is THREAD_ZOMBIE, 
    if(thread[threadId].status == THREAD_ZOMBIE){

      //    Change its status to THREAD_UNUSED
      thread[threadId].status = THREAD_UNUSED;

      //    Set *status to its exit code, and return 0
      *status = thread[threadId].exitValue;
      return 0;
    }
    // Otherwise, the thread was joined by someone else already; return -1    
    return -1;
}

// Terminate execution of current thread
void ut_finish(int result)
{
    // record <result> in current thread's exitValue field
    thread[curThread].exitValue = result;
    // set current thread's status to THREAD_ZOMBIE
    thread[curThread].status = THREAD_ZOMBIE;
    // pick another thread to run
    ut_yield();
}