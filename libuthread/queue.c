#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

//linked list 
struct node{
	struct node *next;
	int data;
};

struct queue {
	/* TODO Phase 1 */
	int size;
	struct node *head;
	struct node *tail;

};

queue_t queue_create(void) // the data type at the begining is the return type!!
{
	/* TODO Phase 1 */
	queue_t theQueue = malloc(sizeof(struct queue));
	if(theQueue == NULL){
		return NULL;
	}

	theQueue->head = NULL;
	theQueue->head = NULL;
	theQueue->size = 0;

	return theQueue;
}

int queue_destroy(queue_t queue)
{
	/* TODO Phase 1 */
}

int queue_enqueue(queue_t queue, void *data) //add new data to the back of line 
{
	/* TODO Phase 1 */
}

int queue_dequeue(queue_t queue, void **data) // remove the data 
{
	/* TODO Phase 1 */
}

int queue_delete(queue_t queue, void *data) //remove an element 
{
	/* TODO Phase 1 */
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	/* TODO Phase 1 */
}

int queue_length(queue_t queue)
{
	/* TODO Phase 1 */
}

