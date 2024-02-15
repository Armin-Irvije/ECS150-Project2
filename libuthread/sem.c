#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"
#include "sem.h"
#include "private.h"
#include "uthread.h"

int BUM = 1;

struct semaphore {
    int count;
	queue_t waitingQ;
};

sem_t sem_create(size_t count) {

    sem_t semaphore = (sem_t)malloc(sizeof(struct semaphore)); //allocate memory 
    if (semaphore == NULL) return NULL;
    semaphore->count = count;
	semaphore->waitingQ = queue_create(); //create the semaphore 

	if(BUM)printf("created semaphore\n");
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
	
	if(BUM)printf("inside sem_down\n");
	struct uthread_tcb *runningThread = uthread_current();
    if (sem == NULL) return -1;

    while(sem->count == 0){//the is the gate at the door if we are 0
		if(BUM)printf("inside if about to block\n");
		struct uthread_tcb *runningThread = uthread_current();
		queue_enqueue(sem->waitingQ, runningThread);
		if(BUM)printf("enqueued the running thread\n");
		uthread_block(); //block current running thread
	}
	// if(sem->waitingQ == NULL){
	// 	//enable_preemption()
	// 	return -1;
	// }
	sem->count--;
	if(BUM)printf("decresed sem count: %d\n", sem->count);


    return 0;
}

//Unblock a waiting thread
//dequee
int sem_up(sem_t sem) {
	if(BUM)printf("inside sem_up\n");
	struct uthread_tcb *unblockedThread = NULL;
    if (sem == NULL) return -1;
	
	sem->count++; 
	if(BUM)printf("increased sem count: %d\n", sem->count);

	if(sem->waitingQ != NULL && (sem->waitingQ != NULL)){ //NULL has nothing to do with if there are nodes inside
		struct uthread_tcb *unblockedThread = NULL;
		if(BUM)printf("inside if about to dequeue ready thread semQ size : %d\n", queue_length(sem->waitingQ));
		queue_dequeue(sem->waitingQ, (void **)&unblockedThread);//dequeue the unblocked thread from passed in semaphore
		uthread_unblock(unblockedThread); //call unblocked on it so it can be put into readyQ
	}
	
    return 0;
}