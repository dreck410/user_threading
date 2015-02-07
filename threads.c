//threads.c

#include <assert.h>
#include <ucontext.h>

#include "ut_threads.h"

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
  int i;
    
  // setup stack pointers
  for (i = 0 ; i < MAX_THREADS; ++i ) {
    thread[i].context.uc_stack.ss_sp = stackbuf + i * STACK_SIZE;
    thread[i].context.uc_stack.ss_size = STACK_SIZE;
  }
  
  // initialize main thread
  thread[0].status = THREAD_ALIVE;
  curThread = 0; 
}

// Creates a thread with entry point set to <entry>
// <arg> is the argument that will be passed to <entry> in the new thread
// Returns threadID on success, or -1 on failure (thread table full)
int ut_create(void (* entry)(int), int arg)
{
    
    // Find a slot in thread table whose status is THREAD_UNUSED
    
    // Return -1 if all slots are in use
    
    // Otherwise, set the status of the slot to THREAD_ALIVE
    // and initialize its context
    
    // Return the thread Id
}

// scheduler - picks a thread to run (possibly this one, if no other threads are available)
void ut_yield()
{
 
  // find a thread that can run, using round robin scheduling; pick this one if no other thread can run
  
  // if another thread can run, switch to it
  
  // if no threads are ALIVE, exit the program
    
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
    
    // busy-wait (calling ut_yield in a loop) while the status of thread <threadId> is THREAD_ALIVE
    
    // If the thread status is THREAD_ZOMBIE, 
    //    Change its status to THREAD_UNUSED
    //    Set *status to its exit code, and return 0
    // Otherwise, the thread was joined by someone else already; return -1    
    
}

// Terminate execution of current thread
void ut_finish(int result)
{
    // record <result> in current thread's exitValue field
    // set current thread's status to THREAD_ZOMBIE
    // pick another thread to run
}