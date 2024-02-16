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

void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL); //tests if a queue is created
}

void test_queue_simple(void) 
{
	int d = 3, *p;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &d);
	queue_dequeue(q, (void**)&p);
	TEST_ASSERT(p == &d); //tests if we can store one piece of data into a queue and then read it out of the queue
}

void test_dequeue_empty(void)
{
    queue_t q;
    int *p;

    fprintf(stderr, "*** TEST dequeue_empty ***\n");

    q = queue_create();

    TEST_ASSERT(queue_dequeue(q, (void**)&p) == -1); //tests that ensures that if we try to dequeue from an empty queue we recieve a -1 return value
}

void test_queue_multiple(void)
{
	int d1 = 1;
	int d2 = 2;
	int d3 = 3;
	int *p1;
	int *p2;
	int *p3;
	queue_t q;

	fprintf(stderr, "*** TEST queue_multiple ***\n");

	q = queue_create();
	queue_enqueue(q, &d1);
	queue_enqueue(q, &d2);
	queue_enqueue(q, &d3);
	queue_dequeue(q, (void**)&p1);
	queue_dequeue(q, (void**)&p2);
	queue_dequeue(q, (void**)&p3);
	
	TEST_ASSERT(*p1 == d1);
	TEST_ASSERT(*p2 == d2);
	TEST_ASSERT(*p3 == d3); //tests if we can store multiple data into the queue and dequeue it appropriatley
}

typedef struct {

    int orderid;
    char meal[50];

}fastfood_t;

void test_custom_data_types(void)
{
    fastfood_t order1 = {1, "fries"};
	fastfood_t order2 = {2, "shake"};
	fastfood_t order3 = {3, "burger"};
    fastfood_t *p;
    queue_t q;

    fprintf(stderr, "*** TEST custom_data_types ***\n");

    q = queue_create();
    queue_enqueue(q, &order1);
    queue_enqueue(q, &order2);
    queue_enqueue(q, &order3);

    queue_dequeue(q, (void**)&p);
    TEST_ASSERT(p->orderid == 1 && strcmp(p->meal, "fries") == 0);
    queue_dequeue(q, (void**)&p);
    TEST_ASSERT(p->orderid == 2 && strcmp(p->meal, "shake") == 0);
    queue_dequeue(q, (void**)&p);
    TEST_ASSERT(p->orderid == 3 && strcmp(p->meal, "burger") == 0); //tests the queues ability enqueue and dequeue data structures
}

void test_enqueue_null(void)
{
    queue_t q;

    fprintf(stderr, "*** TEST enqueue_null ***\n");

    q = queue_create();

    TEST_ASSERT(queue_enqueue(q, NULL) == -1); //tests that if we try to enqueue NULL into the queue we return -1
}

void test_dequeue_null(void)
{
    int *ptr;

    fprintf(stderr, "*** TEST dequeue_null ***\n");

    TEST_ASSERT(queue_dequeue(NULL, (void**)&ptr) == -1); //tests that if we try to dequeue from a NULL queue we return -1
}

int main(void)
{
	test_create();
    test_queue_simple();
	test_dequeue_empty();
    test_queue_multiple();
    test_custom_data_types();
    test_enqueue_null();
    test_dequeue_null();
	
	return 0;
}



