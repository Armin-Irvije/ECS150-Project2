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

static bool preempt_enabled = true;
void preempt_enable(void) {
    preempt_enabled = true;
}//context.c does not compile without this

int DEBUG = 0;
queue_t ReadyQ = NULL;
queue_t BlockedQ = NULL;

enum thread_states
{
	RUNNING,
	BLOCKED,
	READY,
};

struct uthread_tcb {
	/* TODO Phase 2 */
	struct uthread_tcb *next;
	uthread_ctx_t context;
	enum thread_states state;
	int threadnum;
	uthread_func_t function;
	void *arg;
	int toggleQ;
	int semtarget;
	int blocked;
	int special;

};

struct TCBLL // linked list of TCB
{
	int size;
	struct uthread_tcb *head;
	struct uthread_tcb *tail;
};

struct TCBLL *theTCBLL;
int threadcount = 1;
int exitToggle = 0;

void add(struct uthread_tcb *new_tcb) {

    if (theTCBLL->tail == NULL) {
        // If the list is empty, set the new_tcb as both head and tail
        theTCBLL->head = new_tcb;
        theTCBLL->tail = new_tcb;
    } else {
        // Append the new_tcb to the tail of the list
       theTCBLL->tail->next = new_tcb;
       theTCBLL->tail = new_tcb;
    }

    // Update the size of the list
    theTCBLL->size++;
}

struct uthread_tcb *uthread_current(void){

	if(DEBUG)printf("inside uthread_current\n");

	struct uthread_tcb *currentTCB = theTCBLL->head;
	
	while (1)
	{
		
		if (currentTCB->state == RUNNING)
		{
			if(DEBUG)printf("found running thread return as current\n");
			if(DEBUG)printf("the running thread is equal to %d\n", currentTCB->threadnum);
			break;
		}
		else
		{
			
			currentTCB = currentTCB->next;
		}
	}

	struct uthread_tcb *runningTCB = currentTCB;

	if (currentTCB->semtarget > theTCBLL->size - 1){
		return runningTCB;
	}

	if (currentTCB->toggleQ){
		if(DEBUG)printf("inside sem toggle\n");
		while(currentTCB->semtarget != runningTCB->threadnum){
			if(DEBUG)printf("sem target is %d and found thread is %d\n", currentTCB->semtarget, runningTCB->threadnum);
			runningTCB = runningTCB->next;
			if (runningTCB == NULL){
				runningTCB = theTCBLL->head;
			}
		}
		currentTCB->toggleQ = 0;
		return runningTCB;
	}

	return currentTCB;

	return 0;

}

// Scheduler to switch to the next available thread
static void uthread_scheduler(void) {


	//get and strore current running thread
	//if calling block function, store current running thread into ready queue, do context switch in blocked, update ready queue()  because the state of a thread might have been changed by the sem, context switch from running thread to the thread that was dequeued from the ready queue
	//if calling unblock function, delete the thread that we are unblocking from the blocked queu update blocked queue() and set the state of the thread to ready and update the ready queue()
	if(DEBUG)printf("inside scheduler\n");
	struct uthread_tcb *next_thread = NULL;
	struct uthread_tcb *curr = uthread_current(); //this causes issue when current needs to be set to blocked
	if(DEBUG)printf("about to dequeue and set next thread\n");
    if (queue_dequeue(ReadyQ, (void **)&next_thread) != 0) return; // No thread to switch to
	next_thread->state = RUNNING; 
	if(DEBUG)printf("we dequeued ready thread, queue size: %d\n", queue_length(ReadyQ));
	curr->state = READY;
	if(DEBUG)printf("the current thread is equal to %d\n", curr->threadnum);
	if (!curr->blocked){
		queue_enqueue(ReadyQ, curr); // Enqueue the current thread back into the ready queue
		if(DEBUG)printf("the next thread is equal to %d\n", next_thread->threadnum);
		if(DEBUG)printf("put curr back into ready queue about to context switch, queue size: %d\n", queue_length(ReadyQ));
	}
	//if()printf("found running thread return as current\n");
    uthread_ctx_switch(&(curr->context), &(next_thread->context));
}

void uthread_yield(void)//this is fine cause threads will call yield themselves!!
{
	/* TODO Phase 2 */
	if(DEBUG)printf("inside yield\n");
	uthread_scheduler();
}

void uthread_exit(void)
{
	/* TODO Phase 2 */
	if(DEBUG)printf("inside exit\n");
	struct uthread_tcb *current = uthread_current();
	struct uthread_tcb *next_thread = uthread_current();
	if(DEBUG)printf("found thread %d inside exit\n", current->threadnum);
	if (current != NULL) {
		if(DEBUG)printf("about to destroy stack\n");
        //uthread_ctx_destroy_stack(current->context.uc_stack.ss_sp);
		if(DEBUG)printf("about to free exited threads\n");
        //free(current);
        // current = NULL;
    }

	if(queue_length(ReadyQ) == 0){
		if(DEBUG)printf("adding idle thread to queue\n");
		queue_enqueue(ReadyQ,theTCBLL->head);
	}
	if (queue_dequeue(ReadyQ, (void **)&next_thread) != 0) return;
	next_thread->state = RUNNING;
	queue_delete(ReadyQ, current);
	exitToggle = 1;
	uthread_scheduler();
}

int uthread_create(uthread_func_t func, void *arg) //let create be a stand alone create function no if statements
{//i don't believe we run this any where else in the API
	if(DEBUG)printf("inside uthread create\n");
	struct uthread_tcb *newThread = malloc(sizeof(struct uthread_tcb));
	if (uthread_ctx_init(&newThread->context, uthread_ctx_alloc_stack(), func, arg) != 0) return -1;
	newThread->state = READY;
	newThread->function = func;
	newThread->arg = arg;
	queue_enqueue(ReadyQ, newThread); //put thread into ready Q
	newThread->threadnum = threadcount + 1;
	threadcount = threadcount + 1;
	add(newThread);//add to linked list
	if(DEBUG)printf("the created thread is equal to %d\n", newThread->threadnum);
	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg) 
{
	if(preempt)printf("preempt activated\n");
	if(DEBUG)printf("inside uthread run!\n");

	ReadyQ = queue_create();
	BlockedQ = queue_create();

	//initialize linked list
	theTCBLL = malloc(sizeof(struct TCBLL));
	theTCBLL->head = NULL; // initializing linked list head tail and size
	theTCBLL->tail = NULL;
	theTCBLL->size = 0;
	if(DEBUG) printf("initialzied linked list!\n");

	//create idle thread 
	struct uthread_tcb *idleTCB = malloc(sizeof(struct uthread_tcb));
	if (uthread_ctx_init(&idleTCB->context, uthread_ctx_alloc_stack(), func, arg) != 0) return -1;
	idleTCB->state = READY;
	idleTCB->function = NULL;
	idleTCB->arg = NULL;
	idleTCB->threadnum = 0;
	add(idleTCB);
	if(DEBUG)printf("idleThread initialized\n");

	//create initial thread
	struct uthread_tcb *initialTCB = malloc(sizeof(struct uthread_tcb)); 
	if (uthread_ctx_init(&initialTCB->context, uthread_ctx_alloc_stack(), func, arg) != 0) return -1;
	initialTCB->state = RUNNING;
	initialTCB->function = func;
	initialTCB->arg = arg;
	initialTCB->threadnum = threadcount;
	add(initialTCB);
	if(DEBUG)printf("initial thread created, list size: %d\n", theTCBLL->size);
	initialTCB->function(initialTCB->arg); //runs the intial thread
	//it makes sense to just run the initial thread because it set everything in motion
	if(DEBUG)printf("back in idle thread queue size : %d\n", queue_length(ReadyQ));
	while(queue_length(ReadyQ) != 0){

		uthread_scheduler();
		
	}

	return 0;
	
}

void uthread_block(void)
{
	struct uthread_tcb *curr = uthread_current();
	if(DEBUG)printf("inside uthread_block\n");
	//update blocked Q by checking through the linked list to see if any of the threads have a state set to blocked
	if(DEBUG)printf("about to call yield from blocked\n");
	curr->blocked = 1;
	uthread_yield(); 
}
//void uthread_unblock(struct uthread_tcb *uthread)
void uthread_unblock(struct uthread_tcb *uthread)
{
	if(DEBUG)printf("inside uthread_Unblocked\n");
	if (uthread != NULL) {
        uthread->state = READY;
		if(DEBUG)printf("we set unblocked thread %d state to READY\n", uthread->threadnum);
        queue_enqueue(ReadyQ, uthread);  // Enqueue to ready queue
		if(DEBUG)printf("queued unblocked thread %d into ready\n", uthread->threadnum);
    }
	if(DEBUG)printf("tried to unblock a NULL thread\n");
}

