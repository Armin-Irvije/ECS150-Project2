# Uthread Library
## Summary
The project implements a basic user-level thread library. The aim is to  
provide a comprehensive interface for threads and synchronization. First part 
is queue API, which employs a node-based structure to manage thread scheduling
efficiently. Threads are created and managed through the uthread API, which 
implements the execution of independent threads. The library offers a 
Semaphore API for thread synchronization, enabling the for resources to be 
shared among concurrent threads. 

## Design
The design of the program follows a modular approach, with each file `queue.c`,
`uthread.c`, `sem.c` capturing a functionality crucial for thread 
management and synchronization. This design allows for clear separation and 
promotes good code organization. Each file exposes a set of functions through 
its corresponding header file enabling other parts of the library to utilize 
these APIs. The `private.h` header file contains definitions 
that are internal to the library and not meant for the users, enforcing 
abstraction. The `Makefile` makes use of a build automation tool, allowing for 
compilation and maintenance of the library. This design promotes code and 
maintainability making it suitable for building robust multithreaded 
applications.

## Queue
The queue implementation in our project uses a classic node-based structure
to manage data insertion, deletion, and traversal efficiently. The design 
uses a linked list of nodes, where each node contains a pointer to
the next node and the data it holds. This design choice allows for dynamic 
allocation and deallocation of memory, helping with scalability and 
flexibility in managing various data types. By maintaining pointers to both 
the head and tail nodes, enqueue and dequeue operations have constant time 
complexity, making sure we have the best performance regardless of the queue's 
size. The API also supports operations such as `queue_deletion` for when data 
may need to be removed based on certain condtions. `queue_iteration` could be 
useful for traversal and filtering. The two functions boost the overall 
robustness of the implementation.

## uthread
The uthread.c file implements important functionalities for managing threads   
within the thread library. The `uthread_tcb` struct represents a thread 
control block and includes variables such as next for linking TCBs in a 
linked list, context for storing the execution context of the thread, state for
tracking its state (RUNNING, READY, BLOCKED) This is good at tracking and 
management of individual threads within the system. The TCBLL struct is a 
linked list of TCBs and provides the ability to add and manage threads in the 
system. With the linked list we can track the current running thread inside 
`uthread_current()` easily and find the idle thread which is the always the 
head of the list. The linked list also overall helps with dynamic allocation 
and deallocation of threads as needed.

The `uthread_scheduler()` function serves as the core, responsible for 
scheduling and switching between threads in the system. When yield is called,
the scheduler selects the next available thread to run from the ready queue,
prioritizing threads based on scheduling policy, in this case FIFO policy. The 
function dequeues the selected thread from the ready queue, set its state as 
RUNNING, and enqueues the previously executing thread back into the ready 
queue. The scheduler invokes the context switch operation to transition 
execution from the current thread to the selected thread.

The other functions implemented in `uthread.c`, such as `uthread_exit` and 
`uthread_block`, are used for termination, and blocking. These functions 
leverage the underlying thread structures and queue operations to ensure proper
execution of threads in the system. By having the thread management logic into 
reusable functions, `uthread.c` promotes ease of maintenance.

## Semaphores
The `sem.c` file provides semaphore functionalities within the thread library, 
which implements synchronization and resource management. The file defines a 
semaphore struct representing a semaphore object, which includes fields for 
count, semaphore number, and a waiting queue to hold threads waiting for 
access to the semaphore. The `sem_create` function allocates memory for a 
semaphore, initializes its fields, and returns a pointer to the created 
semaphore. The `sem_down` function is responsible for decrementing the 
semaphore count and blocking threads by calling `uthread_block()` when the 
semaphore count reaches zero, making sure there is mutual exclusion and 
preventing race conditions. The `sem_up` function increments the semaphore 
count and unblocks waiting threads by calling `uthread_unblock`, allowing them
to proceed with their execution. 

## Testing
`queue_tester.c` offers a comprehensive set of test on the queue 
implementation, testing crucial capabilites and edge cases to make sure the 
API is reliable. Through a series of test cases, it examines the creation of 
queues, enqueueing and dequeueing operations with both simple and multiple 
elements, handling of empty queues, and creation of custom data types. The 
tests also cover scenarios such as dequeueing from an empty queue, enqueueing 
NULL pointers, addressing potential failure points, corner cases and error 
handling mechanisms. The tester serves as a vital tool for verifying the 
correctness of the queue implementation, promoting confidence in its usage in 
the library. 

The other provided test files also helped in validating the functionality 
and reliability of the uthread and semaphore implementation. The 
`uthread_yield.c` demonstrates the correct sequencing of threads creation and 
yielding, checking that we properly have the parent thread returns before its 
child threads execute. This test validates the core of our ability to context 
switch properly. The `sem_simple.c` focuses on synchronization among multiple 
threads, testing the proper ordering of thread execution using semaphores. It 
tests the accuracy of the `sem_down`, `sem_up`, `uthread_blocked`, 
`uthread_unblocked`. Overall, these tests comprehensively assess the essential 
aspects of the Uthread  and semaphore implementation, providing confidence in 
its correctness in synchronized multithreading.

## Struggles 
In our attempt to implement preemption within our system, we encountered 
numerous logic flaws that impacted our ability to pass critical test cases like
`uthread_yield` and `sem_simple.c`. Our struggles were centered around our 
fundamental uncertainties surrounding the behavior of our uthread logic. 
Despite our best efforts, we found it challenging to figure out the precise 
handling of scenarios such as thread exits and the integration of the idle 
thread into the ready queue. Our apporach to Preemptive API would have the 
implemenation seamlessly interrupt running threads at predefined intervals, 
ensuring fair scheduling. This would involve `preempt_disable()` and 
`preempt_enable()` functions to respectively halt and resume preemption, along
with `preempt_start()` and `preempt_stop()` to begin and conclude preemption.

## Conclusion
The Uthread library presents a good solution for user-level thread management 
and synchronization. The similarities with the POSIX pthread library lie in the 
fundamental concepts of thread creation, synchronization primitives, and 
context switching. Key concepts learned from this project include thread 
management, synchronization mechanisms, and the importance of comprehensive 
testing. The modular design, and testing demonstrate the library's reliability 
and ease of use. The queue API, implemented with a node structure, ensures 
efficient thread scheduling and management, Whereas the uthread API implements 
the execution of threads with crucial functions like `uthread_create` and 
`uthread_yield`. The Semaphore API allows for coordination of shared resources 
among concurrent threads, incorporating mutual exclusion and preventing race 
conditions. The thorough testing files, validates the reliability of the library's 
implementations. Working at the challenges encountered with preemption would enhance
the library's capabilities, providing more responsiveness in execution scenarios.
The Uthread library  is a valuable tool for developers seeking efficient and 
reliable thread management and synchronization in their projects.
