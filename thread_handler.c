#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "alarm_handler.h"
#include "thread_handler.h"
#include "queue.h"

#define DELAY 10000
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/* The two macros are extremely useful by turnning on/off interrupts when atomicity is required */
#define DISABLE_INTERRUPTS() {  \
    asm("wrctl status, zero");  \
}

#define ENABLE_INTERRUPTS() {   \
    asm("movi et, 1");          \
    asm("wrctl status, et");    \
}

/* the current running thread */
static tcb *current_running_thread      = NULL;
static Q_type *runQueue = NULL;

/* pointing to the stack/context of main() */
static unsigned int *main_stack_pointer = NULL;

tcb *get_current_running_thread()
{
	return current_running_thread;
}

void run_queue_init()
{
	runQueue = Queue(NULL, NULL, 0);
}

Q_type *get_running_queue()
{
	return (Q_type*)runQueue;
}

tcb *mythread_create(unsigned int tid, unsigned int stack_size, void (*mythread)(unsigned int tid))
{
	//printf("inside\n");
    unsigned int *tmp_ptr;
    
    /* allocate a tcb for a thread */
    tcb *thread_pointer;
    
    thread_pointer                      = (tcb *)malloc(sizeof(tcb));
    if (thread_pointer == NULL)
    {
        printf("Unable to allocate space!\n");
        exit(1);
    }
    
    /* initialize the thread's tcb */
    thread_pointer->tid                 = tid;
    thread_pointer->stack               = (unsigned int *)malloc(sizeof(unsigned int) * stack_size);
    if (thread_pointer->stack == NULL)
    {
        printf("Unable to allocate space!\n");
        exit(1);
    }
    thread_pointer->stack_size          = stack_size;
    thread_pointer->stack_pointer       = (unsigned int *)(thread_pointer->stack + stack_size - 19);
    thread_pointer->state               = NEW;
    
    /* initialize the thread's stack */
    tmp_ptr                             = thread_pointer->stack_pointer;
    tmp_ptr[18]                         = (unsigned int)mythread;                               // ea
    tmp_ptr[17]                         = 1;                                                    // estatus
    tmp_ptr[5]                          = tid;                                                  // r4
    tmp_ptr[0]                          = (unsigned int)mythread_cleanup;                       // ra
    tmp_ptr[-1]                         = (unsigned int)(thread_pointer->stack + stack_size);   // fp
           
    return thread_pointer;
}

/* NEW ----> READY */
void mythread_start(tcb *thread_pointer)
{
	//printf("I started\n");
    // assert(thread_pointer && thread_pointer->state == NEW);
    thread_pointer->state = READY;
}

/* READY --push into--> readyQ */
void mythread_join(tcb *thread_pointer)
{
	//printf("I joined\n");
    // assert(thread_pointer && thread_pointer->state == READY);
    enqueue((void *)thread_pointer, runQueue);
}

/* RUNNING ----> BLOCKED */
void mythread_block(tcb *thread_pointer)
{
    // assert(thread_pointer && thread_pointer->state == RUNNING);
    thread_pointer->state = BLOCKED;
}

/* RUNNING ----> TERMINATED */
void mythread_terminate(tcb *thread_pointer)
{
    // assert(thread_pointer && thread_pointer->state == RUNNING);
    thread_pointer->state = TERMINATED;
}

void *mythread_schedule(void *context)
{
    if (getQsize(runQueue) > 0)
    {
        if (current_running_thread != NULL)
        {
            // assert(current_running_thread->state == RUNNING);
            // assert(main_stack_pointer != NULL);
            current_running_thread->state = READY;
            current_running_thread->stack_pointer = (unsigned int *)context;
            enqueue(current_running_thread, runQueue);
        }
        else if (main_stack_pointer == NULL)
        {
            main_stack_pointer = (unsigned int *)context;
        }
        
        do
        {
        	current_running_thread = (tcb *)dequeue(runQueue);
        	if(current_running_thread->state == BLOCKED)
        	{
        		enqueue(current_running_thread, runQueue);
        	}
        } while (current_running_thread->state == BLOCKED);

        // assert(current_running_thread->state == READY);
        current_running_thread->state = RUNNING;
        
        context = (void *)(current_running_thread->stack_pointer);
    }
    else if (current_running_thread==NULL && main_stack_pointer!=NULL)
    {        
        context = (void *)main_stack_pointer;
    }

    return context;
}

unsigned int mythread_isQempty()
{
    return (getQsize(runQueue) == 0) && (current_running_thread == NULL);
}

void mythread_cleanup()
{
	int count = 0;
    DISABLE_INTERRUPTS();
    mythread_terminate(current_running_thread);
    free(current_running_thread->stack);
    free(current_running_thread);
    current_running_thread = NULL;
    ENABLE_INTERRUPTS();
    while(1){
    	for (count = 0; count < DELAY; count++);
    	printf(".");
    }


}
