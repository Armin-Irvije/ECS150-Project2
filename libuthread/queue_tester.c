#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "queue.h"
//#include <queue.h>

//this one is graded!!
#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

// ..........More added test cases...............

/* Enqueue/Dequeue multiple elements */
void test_queue_multiple(void)
{
	int data1 = 1, data2 = 2, data3 = 3, *ptr1, *ptr2, *ptr3;
	queue_t q;

	fprintf(stderr, "*** TEST queue_multiple ***\n");

	q = queue_create();
	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3);

	queue_dequeue(q, (void**)&ptr1);
	queue_dequeue(q, (void**)&ptr2);
	queue_dequeue(q, (void**)&ptr3);

	TEST_ASSERT(ptr1 == &data1);
	TEST_ASSERT(ptr2 == &data2);
	TEST_ASSERT(ptr3 == &data3);
}

/* Test queue operations on an empty queue */
void test_empty_queue(void)
{
	queue_t q;
	int *ptr;

	fprintf(stderr, "*** TEST empty_queue ***\n");

	q = queue_create();

	// Dequeue from an empty queue should return NULL
	TEST_ASSERT(queue_dequeue(q, (void**)&ptr) == -1); //pass this test

	// Enqueue and dequeue from an empty queue should give the same pointer
	queue_enqueue(q, &ptr);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == NULL); //this does not pass
}

/* Test queue operations with custom data types */
typedef struct {
    int id;
    char name[20];
} person_t;

void test_custom_data_types(void)
{
    person_t person1 = {1, "Alice"}, person2 = {2, "Bob"}, person3 = {3, "Charlie"};
    person_t *ptr;
    queue_t q;

    fprintf(stderr, "*** TEST custom_data_types ***\n");

    q = queue_create();
    queue_enqueue(q, &person1);
    queue_enqueue(q, &person2);
    queue_enqueue(q, &person3);

    queue_dequeue(q, (void**)&ptr);
    TEST_ASSERT(ptr->id == 1 && strcmp(ptr->name, "Alice") == 0);

    queue_dequeue(q, (void**)&ptr);
    TEST_ASSERT(ptr->id == 2 && strcmp(ptr->name, "Bob") == 0);

    queue_dequeue(q, (void**)&ptr);
    TEST_ASSERT(ptr->id == 3 && strcmp(ptr->name, "Charlie") == 0);
}
/* Test dequeue from an empty queue */
void test_dequeue_empty(void)
{
    queue_t q;
    int *ptr;

    fprintf(stderr, "*** TEST dequeue_empty ***\n");

    q = queue_create();

    // Dequeue from an empty queue should return -1
    TEST_ASSERT(queue_dequeue(q, (void**)&ptr) == -1);
}

/* Test enqueueing NULL pointers */
void test_enqueue_null(void)
{
    queue_t q;

    fprintf(stderr, "*** TEST enqueue_null ***\n");

    q = queue_create();

    // Enqueueing a NULL pointer should return -1
    TEST_ASSERT(queue_enqueue(q, NULL) == -1);
}

/* Test dequeue from a NULL queue */
void test_dequeue_null(void)
{
    int *ptr;

    fprintf(stderr, "*** TEST dequeue_null ***\n");

    // Dequeue from a NULL queue should return -1
    TEST_ASSERT(queue_dequeue(NULL, (void**)&ptr) == -1);
}


int main(void)
{
	test_create();
    test_queue_simple();
    test_queue_multiple();
    test_empty_queue();
    test_custom_data_types();
    test_dequeue_empty();
    test_enqueue_null();
    test_dequeue_null();
	
	return 0;
}



