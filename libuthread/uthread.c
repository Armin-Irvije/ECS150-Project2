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

int DEBUG = 1;
queue_t readyQ = NULL;
queue_t blockedQ = NULL;

enum thread_states
{
	RUNNING,
	BLOCKED,
	READY,
	ZOMBIE
};

struct uthread_tcb {
	/* TODO Phase 2 */
	struct uthread_tcb *next;
	uthread_ctx_t context;
	enum thread_states state;
	uthread_func_t function;
	void *arg;

};
struct TCBLL // linked list of TCB
{
	int size;
	struct uthread_tcb *head;
	struct uthread_tcb *tail;
};
struct TCBLL *theTCBLL;

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

static void uthread_loop(void) {
	// while (1) {
    //     if (queue_length(readyQ) == 0)
    //         break;  // No more threads to run

    //     uthread_scheduler();  // Yield to the next available thread
    // }
    
}

struct uthread_tcb *uthread_current(void){

	// we need to know which tcb in the linked list is running
	struct uthread_tcb *currentTCB = theTCBLL->head;
	if(DEBUG)printf("inside uthread_current\n");
	while (currentTCB->next != NULL)
	{
		if(DEBUG)printf("checking whos running\n");
		if (currentTCB->state == RUNNING)
		{
			if(DEBUG)printf("found running thread return as current\n");
			return currentTCB;
		}
		else
		{
			
			currentTCB = currentTCB->next;
		}
	}
	return NULL; //no thread is running

}

// Scheduler to switch to the next available thread
static void uthread_scheduler(void) {

	if(DEBUG)printf("inside scheduler\n");
	struct uthread_tcb *next_thread = NULL;
	struct uthread_tcb *curr = uthread_current();
	if(DEBUG)printf("about to dequeue and set next thread\n");
    if (queue_dequeue(readyQ, (void **)&next_thread) != 0) return; // No thread to switch to
	next_thread->state = RUNNING; 
	if(DEBUG)printf("we dequeued ready thread, queue size: %d\n", queue_length(readyQ));

	curr->state = READY;
	queue_enqueue(readyQ, curr); // Enqueue the current thread back into the ready queue
	if(DEBUG)printf("put curr back into ready queue about to context switch, queue size: %d\n", queue_length(readyQ));
	
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
	if(DEBUG)printf("found current inside exit\n");
	if (current != NULL) {
		if(DEBUG)printf("about to destroy stack\n");
        //uthread_ctx_destroy_stack(current->context.uc_stack.ss_sp);
		if(DEBUG)printf("about to free exited threads\n");
        //free(current);
        current = NULL;
    }
	if(DEBUG)printf("about to dequeue in exit\n");
	struct uthread_tcb *nextRUNNING = NULL;
	if(queue_dequeue(readyQ, (void **)&nextRUNNING) != 0) return;
	nextRUNNING->state = RUNNING;

	if(queue_length(readyQ) == 0){
		if(DEBUG)printf("adding idle thread to queue\n");
		queue_enqueue(readyQ,theTCBLL->head);
	}
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

	add(newThread);//add to linked list
	queue_enqueue(readyQ, newThread); //add to ready q
	if(DEBUG)printf("created new thread! queue size: %d\n", queue_length(readyQ));
}

int uthread_run(bool preempt, uthread_func_t func, void *arg) 
{
	if(DEBUG)printf("inside uthread run!\n");
	//create ready queue
	readyQ = queue_create();
    if (readyQ == NULL) return -1;

	//initialize linked list
	theTCBLL = malloc(sizeof(struct TCBLL));
	theTCBLL->head = NULL; // initializing linked list head tail and size
	theTCBLL->tail = NULL;
	theTCBLL->size = 0;
	if(DEBUG) printf("initialzied linked list!\n");

	//create idle thread 
	struct uthread_tcb *idleTCB = malloc(sizeof(struct uthread_tcb));
	if (uthread_ctx_init(&idleTCB->context, uthread_ctx_alloc_stack(), func, arg) != 0) return -1;
	add(idleTCB);
	//queue_enqueue(readyQ, idleTCB);
	if(DEBUG)printf("idleThread initialized\n");

	//create initial thread
	struct uthread_tcb *initialTCB = malloc(sizeof(struct uthread_tcb)); 
	if (uthread_ctx_init(&initialTCB->context, uthread_ctx_alloc_stack(), func, arg) != 0) return -1;
	initialTCB->state = RUNNING;
	initialTCB->function = func;
	initialTCB->arg = arg;
	add(initialTCB);
	if(DEBUG)printf("initial thread created, list size: %d\n", theTCBLL->size);
	initialTCB->function(initialTCB->arg);
	//it makes sense to just run the initial thread because it set everything in motion
	if(DEBUG)printf("back in idle thread queue size : %d\n", queue_length(readyQ));

	// while(1){

	// 	if(queue_length(readyQ) == 0){
	// 		return 0;
	// 	}else{
	// 		uthread_yield();
	// 	}
	// }
	
}

void uthread_block(void)
{
	/* TODO Phase 3 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
}

