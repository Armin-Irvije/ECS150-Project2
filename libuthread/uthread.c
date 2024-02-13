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

//debug toggle 
int DEBUG = 0;

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
	enum thread_states state; // running, ready, blocked?
	
	uthread_func_t function;
	void *arg;
	void *stackPointer;
	uthread_ctx_t context;
};

struct TCBLL // linked list of TCB
{
	int size;
	int doneRunningCount; // threads that have finished
	struct uthread_tcb *head;
	struct uthread_tcb *tail;
};

// Declare a global instance of TCB linked list
struct TCBLL *theTCBLL; // we might need to fix how we allocate linked list memory

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/3 */
	// we need to know which tcb in the linked list is running
	struct uthread_tcb *currentTCB = theTCBLL->head;

	while (currentTCB->next != NULL)
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
	return NULL; //no thread is running
}

void uthread_yield(void)
{
	struct uthread_tcb *runningTCB = uthread_current();
	runningTCB->state = READY;
	runningTCB->next->state = RUNNING;
	if(DEBUG)printf("current thread yielded, states changed\n");
}

void uthread_exit(void)
{
	/* TODO Phase 2 */
	struct uthread_tcb *curr = uthread_current();
	curr->function(curr->arg);
}

int uthread_create(uthread_func_t func, void *arg)
{
	if(DEBUG) printf("Inside uthread_create, list size: %d\n", theTCBLL->size);

	if (theTCBLL->size == 1) // if we only have the IDLE thread in our TCBLL
	{						 // Create the initial thread

		struct uthread_tcb *initialTCB = malloc(sizeof(struct uthread_tcb)); // new tcb node
		if (initialTCB == NULL) // TCB node memory allocation fails
		{
			return -1;
		}
		
		// all of this is setting the characteristics of the TCBLL
		theTCBLL->size++;
		initialTCB->state = RUNNING;
		initialTCB->function = func;
		initialTCB->arg = arg;
		initialTCB->next = NULL;

		// set new TCB node as tail
		if (theTCBLL->tail != NULL)
		{
			theTCBLL->tail->next = initialTCB;
			theTCBLL->tail = initialTCB;
		}
		
		if(DEBUG) printf("created inital thread\n");
		initialTCB->function(initialTCB->arg);
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
		newTCB->stackPointer = NULL;
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
		if(DEBUG)printf("created new child thread\n");
		return 0;
	}
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{	
	if(DEBUG) printf("inside uthread_run!\n");

	theTCBLL = malloc(sizeof(struct TCBLL));

	if (theTCBLL == NULL) // if linked list memory allocation fails
	{
		return -1;
	}
	
	theTCBLL->head = NULL; // initializing linked list head tail and size
	theTCBLL->tail = NULL;
	theTCBLL->size = 0;
	if(DEBUG) printf("initialzied linked list!\n");

	// registers the so-far single execution flow of the application as the idle thread that the library can later schedule for execution
	struct uthread_tcb *idleTCB = malloc(sizeof(struct uthread_tcb)); // creates the IDLE TCB and allocate the memory

	
	if (idleTCB == NULL) // if IDLE TCB memory allocation fails
	{
		return -1;
	}
	
	theTCBLL->size++; // increments the size of the TCBLL

	idleTCB->state = IDLE;		 // sets the state of IDLE thread to IDLE
	idleTCB->stackPointer = NULL;	 // context saved
	idleTCB->next = NULL;

	theTCBLL->head = idleTCB; // sets the head of the TCBLL to the IDLE thread
	theTCBLL->tail = idleTCB;
	if(DEBUG) printf("created idle thread\n");

	
	// create initial thread
	uthread_create(func, arg); // taken from our uthread_run(arguments)
	if(DEBUG) printf("Returned from uthread create\n");



	struct uthread_tcb *current = malloc(sizeof(struct uthread_tcb));
	current = uthread_current();
	if(DEBUG) printf("stored current thread\n");
	//I think we start executing in this while loop
	while (theTCBLL->size > 0) // "infinite" loop runs as long as there are threads in the list
	{
		if(DEBUG) printf("inside while loop\n");
		struct uthread_tcb *current = uthread_current();
		if(current->state == ZOMBIE){
			//TODO: clean up resouces
			theTCBLL->size--;
			
		}else{
			uthread_yield();
		}
	}
	 
	free(current);
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
