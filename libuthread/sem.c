#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"
#include "sem.h"
#include "private.h"
#include "uthread.h"

int BUM = 0;
int semcounter = 1;

struct semaphore {
    int count;
	int semnum;
	queue_t waitingQ;
};

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
	int threadnum;
	uthread_func_t function;
	void *arg;
	int toggleQ;
	int semtarget;
	int blocked;

};

sem_t sem_create(size_t count) {

	if(count)printf("does this matter\n");
    sem_t semaphore = (sem_t)malloc(sizeof(struct semaphore)); //allocate memory 
    if (semaphore == NULL) return NULL;
    semaphore->count = count;
	semaphore->semnum = semcounter;
	semcounter = semcounter + 1;
	semaphore->waitingQ = queue_create(); //create the semaphore 

	if(BUM)printf("created semaphore %d\n", semaphore->semnum);
    return semaphore;
}

int sem_destroy(sem_t sem) {
    if (sem == NULL) return -1;
	queue_destroy(sem->waitingQ);
    free(sem);
    return 0;
}


// When a thread successfully takes the
// resource, the count is decreased. When the resource is not available,
// following threads are blocked until the resource becomes available again.
//enuque (sem->wait , uthread current)
//check if sem what thread has access to sem 
//making thread wait and run
int sem_down(sem_t sem) {
	
	if(BUM)printf("inside sem_down for sem %d\n", sem->semnum);
    if (sem == NULL) return -1;

	if(BUM)printf("sem %d count is equal to %d\n", sem->semnum, sem->count);
	
    if(sem->count == 0){//the is the gate at the door if we are 0
		if(BUM)printf("inside if about to block\n");
		uthread_block(); //block current running thread
	}
	
	sem->count--;
	if(BUM)printf("decresed sem %d count: %d\n", sem->semnum, sem->count);
    return 0;
}

//Unblock a waiting thread
//dequee
int sem_up(sem_t sem) {
	if(BUM)printf("inside sem_up for sem %d\n", sem->semnum);
	struct uthread_tcb *runningThread = uthread_current();
	struct uthread_tcb *unblockedThread = uthread_current();
	
    if (sem == NULL) return -1;

	runningThread->semtarget = sem->semnum;
	
	runningThread->toggleQ = 1;
	unblockedThread = uthread_current(); //with the toggle Q active, the uthread current function returns the thread that should be blocked
	runningThread->toggleQ = 0;

	sem->count++; 

	if(BUM)printf("unblocked Thread is equal to %d\n", unblockedThread->threadnum);
	if(BUM)printf("increased sem %d count: %d\n", sem->semnum, sem->count);
	uthread_unblock(unblockedThread); //call unblocked on it so it can be put into readyQ

    return 0;
}