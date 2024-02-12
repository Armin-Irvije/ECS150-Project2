#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

enum thread_states
{
	IDLE,
	RUNNING,
	READY,
	EXITED
};

struct uthread_tcb
{
	/* TODO Phase 2 */
	struct uthread_tcb *next;
	uthread_func_t *function;
	void *arg;
	enum thread_states state; // running, ready, blocked?
	void *stackPointer;
	void *programCounter; // back up of the cpu registers
};

struct TCBLL // linked list of TCB
{
	int size;
	int doneRunningCount; // threads that have finished
	struct uthread_tcb *head;
	struct uthread_tcb *tail;
};


// Declare a global instance of TCB linked list
struct TCBLL *theTCBLL = (struct TCBLL *)(sizeof(struct TCBLL));

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/3 */
}

void uthread_yield(void)
{
	/* TODO Phase 2 */
}

void uthread_exit(void)
{
	/* TODO Phase 2 */
}

int uthread_create(uthread_func_t func, void *arg)
{

	if (theTCBLL->size == 1)
	{ // Create the initial thread

		struct uthread_tcb *newTCB = malloc(sizeof(struct uthread_tcb)); // new tcb node

		if (newTCB == NULL)
		{
			return -1;
		}

		theTCBLL->size++;
		newTCB->state = RUNNING;
		newTCB->stackPointer =
		newTCB->programCounter =
		newTCB->next = NULL;

		//set new TCB node as tail
		if (theTCBLL->tail != NULL)
		{
			theTCBLL->tail->next = newTCB;
			theTCBLL->tail = newTCB;
		}
		else
		{
			// first node in the TCBLL
			theTCBLL->head = newTCB;
			theTCBLL->tail = newTCB;
		}

		return 0; //edit
	}

	while (theTCBLL->size != theTCBLL->doneRunningCount + 1)
	{

		struct uthread_tcb *newTCB = malloc(sizeof(struct uthread_tcb));
		if (newTCB == NULL)
			return -1;

		theTCBLL->size++;

		newTCB->state = RUNNING;
		newTCB->stackPointer =
		newTCB->programCounter =
		newTCB->next = NULL;

		if (theTCBLL->tail != NULL)
		{
			theTCBLL->tail->next = newTCB;
			theTCBLL->tail = newTCB;
		}
		else
		{
			// first node in the TCBLL
			theTCBLL->head = newTCB;
			theTCBLL->tail = newTCB;
		}
	}

	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{

	if (theTCBLL == NULL)
	{
		return -1;
	}
    
	theTCBLL->head = NULL;
	theTCBLL->tail = NULL;
	theTCBLL->size = 0;

	// registers the so-far single execution flow of the application as the idle thread that the library can later schedule for execution
	struct uthread_tcb *newTCB;

	if (newTCB == NULL)
	{
		return -1;
	}

	theTCBLL->size++;

	newTCB->state = IDLE;
	newTCB->stackPointer =
	newTCB->programCounter =
	newTCB->next = NULL;

	theTCBLL->head = newTCB;
	theTCBLL->tail = newTCB;

	// create initial thread
	uthread_create(func, arg); // taken from our uthread_run(arguments)

	//theTCBLL->size != theTCBLL->doneRunningCount + 1

	while (1)
	{ // infinite while loop that runs until there are no more threads ready to run in the system

		//struct uthread_tcb *newTCB = malloc(sizeof(struct uthread_tcb));
		if (newTCB == NULL)
		{
			return -1;
		}

		theTCBLL->size++;

		newTCB->state = RUNNING;
		newTCB->stackPointer =
			newTCB->programCounter =
				newTCB->next = NULL;

		if (theTCBLL->tail != NULL)
		{
			theTCBLL->tail->next = newTCB;
			theTCBLL->tail = newTCB;
		}
		else
		{
			// first node in the TCBLL
			theTCBLL->head = newTCB;
			theTCBLL->tail = newTCB;
		}
	}

	return 0;
}

void uthread_block(void)
{
	/* TODO Phase 3 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
}
