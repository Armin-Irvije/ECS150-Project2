#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

// TODO : add error checking?

// linked list
struct node
{
	struct node *next;
	void *nodeData;
};

struct queue
{
	int size;
	struct node *head;
	struct node *tail;
};

queue_t queue_create(void)
{
	queue_t theQueue = malloc(sizeof(struct queue));
	if (theQueue == NULL)
	{
		return NULL;
	}

	theQueue->head = NULL;
	theQueue->tail = NULL;
	theQueue->size = 0;

	return theQueue;
}

int queue_destroy(queue_t queue)
{
	if (queue == NULL){
		return -1;
	}

	while (queue->head != NULL)
	{ // while the head not empty
		struct node *temp = queue->head;
		queue->head = queue->head->next;
		free(temp); // setting each node as head and freeing them all
	}

	free(queue); // free the queue struct

	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL)
		return -1;

	// Create the node
	struct node *newNode = malloc(sizeof(struct node));
	if (newNode == NULL)
		return -1;

	newNode->nodeData = data; // data from the
	newNode->next = NULL;

	if(queue->tail != NULL){
		queue->tail->next = newNode;
		queue->tail = newNode;
	}
	else{
		//first node in the queue
		queue->head = newNode;
		queue->tail = newNode;
	}

	queue->size++;

	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if (data == NULL || queue->head == NULL || queue == NULL  )
	{
		return -1;
	}

	struct node *tempNode = queue->head; //dequeue only happens at the head
	*data = tempNode->nodeData;

	queue->head = queue->head->next; //set NEW head 
	free(tempNode);

	if (queue->head == NULL) //if NEW head is null then our tail is null and we have an empty queue
	{
		queue->tail = NULL;
	}

	queue->size--; 
	return 0;
}

int queue_delete(queue_t queue, void *data) // not constant runtime
{
	if (queue == NULL || data == NULL)
	{
		return -1;
	}

	struct node *currentNode = queue->head;
	struct node *prevNode = NULL;

	// iterate to find the node with the given data
	while (currentNode != NULL)
	{

		if (currentNode->nodeData == data)
		{
			if (prevNode == NULL)
			{
				// if head is to be deleted set next as new head 
				queue->head = currentNode->next;

				if (queue->head == NULL) //if that NEW head is NUll then set our tail to null as well because it was only 1 node in the queue
				{
					queue->tail = NULL;
				}
			}
			else // nodes in the middle of the queue
			{
				prevNode->next = currentNode->next;
				// set prev node next to current node next so we don't lose data
				if (prevNode->next == NULL){ //Check if our NEW prevNode is the tail or not 
					queue->tail = prevNode;
				}
			}

			free(currentNode);
			queue->size--;

			return 0;
		}
		prevNode = currentNode;
		currentNode = currentNode->next;
	}

	return -1; // Data not found
}

int queue_iterate(queue_t queue, queue_func_t func) // not constant runtime
{
	if (func == NULL || queue == NULL)
	{
		return -1;
	}

	struct node *currentNode = queue->head;

	while (currentNode != NULL)
	{

		func(queue, currentNode->nodeData); // func takes a queue and node data as arguments
		currentNode = currentNode->next;	// iterate to the next node
	}
	return 0;
}

int queue_length(queue_t queue)
{
	if (queue == NULL)
	{
		return 1;
	}
	return queue->size;
}