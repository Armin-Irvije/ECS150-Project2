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
	BLOCKED,
	READY,
	ZOMBIE
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
struct TCBLL *theTCBLL = (struct TCBLL *)(sizeof(struct TCBLL)); // we might need to fix how we allocate linked list memory

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/3 */
	// we need to know which tcb in the linked list is running
	struct uthread_tcb *currentTCB;
	currentTCB = theTCBLL->head;

	while (theTCBLL != NULL)
	{

		if (currentTCB->state == RUNNING)
		{
			return currentTCB;
		}
		else
		{
			currentTCB = currentTCB->next;
		}
	}
}

void uthread_yield(void)
{
	struct uthread_tcb *runningTCB = uthread_current();

	runningTCB->state = READY;
	runningTCB->next->state = RUNNING;
}

void uthread_exit(void)
{
	/* TODO Phase 2 */
}

int uthread_create(uthread_func_t func, void *arg)
{

	if (theTCBLL->size == 1) // if we only have the IDLE thread in our TCBLL
	{						 // Create the initial thread

		struct uthread_tcb *newTCB = malloc(sizeof(struct uthread_tcb)); // new tcb node

		if (newTCB == NULL) // TCB node memory allocation fails
		{
			return -1;
		}

		// all of this is setting the characteristics of the TCBLL
		theTCBLL->size++;
		newTCB->state = RUNNING;
		newTCB->function = func;
		newTCB->arg = arg;
		newTCB->stackPointer =
			newTCB->programCounter =
				newTCB->next = NULL;

		// set new TCB node as tail
		if (theTCBLL->tail != NULL)
		{
			theTCBLL->tail->next = newTCB;
			theTCBLL->tail = newTCB;
		}

		return 0; // edit
	}
	else
	{ // Child TCB created by the initial TCB

		struct uthread_tcb *newTCB = malloc(sizeof(struct uthread_tcb));
		if (newTCB == NULL)
			return -1;

		theTCBLL->size++;

		newTCB->state = READY;
		newTCB->function = func;
		newTCB->arg = arg;
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
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{

	if (theTCBLL == NULL) // if linked list memory allocation fails
	{
		return -1;
	}

	theTCBLL->head = NULL; // initializing linked list head tail and size
	theTCBLL->tail = NULL;
	theTCBLL->size = 0;

	// registers the so-far single execution flow of the application as the idle thread that the library can later schedule for execution
	struct uthread_tcb *newTCB = malloc(sizeof(struct uthread_tcb)); // creates the IDLE TCB and allocate the memory

	if (newTCB == NULL) // if IDLE TCB memory allocation fails
	{
		return -1;
	}

	theTCBLL->size++; // increments the size of the TCBLL

	newTCB->state = IDLE;		 // sets the state of IDLE thread to IDLE
	newTCB->stackPointer =		 // context saved
		newTCB->programCounter = // context saved
		newTCB->next = NULL;

	theTCBLL->head = newTCB; // sets the head of the TCBLL to the IDLE thread
	theTCBLL->tail = newTCB;

	// create initial thread
	uthread_create(func, arg); // taken from our uthread_run(arguments)

	struct uthread_tcb *currentTCB;
	currentTCB = theTCBLL->head;

	while (1) // infinite while loop that runs until there are no more threads ready to run in the system
	{
		while (theTCBLL != NULL)
		{

			if (currentTCB->state == ZOMBIE)
			{
				return currentTCB;
			}
			else
			{
				currentTCB = currentTCB->next;
			}
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
