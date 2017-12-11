/*
 * mysem.c
 *
 *  Created on: Nov 12, 2017
 *      Author: witty
 */

#include "mysem.h"
#include "thread_handler.h"


#define DISABLE_INTERRUPTS() {  \
    asm("wrctl status, zero");  \
}

#define ENABLE_INTERRUPTS() {   \
    asm("movi et, 1");          \
    asm("wrctl status, et");    \
}

int semInit(semaphore * sem, int semVal)
{
	/* add your code to initialize your semaphore here */
	sem->value = semVal;
	sem->threadCount = 0;
	sem->blockingQueue = Queue(NULL, NULL, 0);
	return (sem->value != NULL && sem->threadCount != NULL && sem->blockingQueue != NULL); //return 1 if all init succeeds
}

void semDown(semaphore * sem)
{
	if(sem->value == 0) // block thread
	{
		//need to disable interrupts here
		DISABLE_INTERRUPTS();
		sem->threadCount++;
		get_current_running_thread()->state = BLOCKED;
		enqueue(get_current_running_thread(), sem->blockingQueue);
		ENABLE_INTERRUPTS();
		while(get_current_running_thread()->state == BLOCKED);

	}
	DISABLE_INTERRUPTS();
	sem->value--;
	ENABLE_INTERRUPTS();
	/* implement your logic to perform down operation on a semaphore here */
}

void semUp(semaphore * sem)
{	//we do this out here because it's a binary semaphore

	DISABLE_INTERRUPTS();
	sem->value++;
	//TODO Need to disable interrupts for this bit
	while(sem->threadCount)
	{

		tcb *thread_to_unblock = dequeue(sem->blockingQueue);
		thread_to_unblock->state = READY;
		sem->threadCount--;
	}
	ENABLE_INTERRUPTS();
	/* implement your logic to perform up operation on a semaphore here */
}

unsigned int semValue(semaphore * sem)
{
	return sem->value;
}

unsigned int semThreadCount(semaphore * sem)
{
	return sem->threadCount;
}


