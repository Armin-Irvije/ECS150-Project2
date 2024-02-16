# Uthread Library
## Summary
The project implements a basic user-level thread library. The aims is to 
provide a comprehensive interface for threads and synchronization. first part 
is Queue API, which employs a node-based structure to manage thread scheduling 
efficiently in a round-robin style. Threads are created and managed through the 
uthread API, which ensures seamless execution of independent threads. The 
library offers a Semaphore API for thread synchronization, enabling the 
coordination of shared resources among concurrent threads. Preemption is 
achieved through interrupt-based scheduling, allowing threads to be preempted, 
ensuring fair CPU allocation among threads. By integrating these components, 
the library offers a comprehensive framework for developing multi-threaded 
applications.

## Design
The design of the program follows a modular approach, with each file `queue.c`,
`uthread.c`, `sem.c` capturing a functionality crucial for thread 
management and synchronization. This design allows for clear separation and 
promotes good code organization. Each file exposes a set of functions through 
its corresponding header file enabling other parts of the library to utilize 
these APIs. The `private.h` header file contains declarations and definitions 
that are internal to the library and not meant to be exposed to external users,
enforcing encapsulation and abstraction. The `Makefile` makes use of a build 
automation tool, allowing for  compilation and maintenance of the library. This
design promotes code maintainability, extensibility, and ease of use, making it
suitable for building robust multithreaded applications.

## Queue
The queue implementation in our project utilizes a classic node-based structure
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
within the thread library. It uses some key structures for the thread 
management, including the uthread_tcb struct and the TCBLL struct. The 
`uthread_tcb` struct represents a thread control block and includes variables 
such as next for linking TCBs in a linked list, context for storing the 
execution context of the thread, state for tracking its state 
(RUNNING, READY, BLOCKED) and `readyQ` for holding threads that are ready to 
run. This is good at tracking and management of individual threads within the 
system. The TCBLL struct is a linked list of TCBs and provides mechanisms for 
adding and managing threads in the system. With the linked list we can track 
the current running thread inside `uthread_current()` easily and find the idle 
thread which is the always the head of the list. This overall helps with 
dynamic allocation and deallocation of threads as needed.

The `uthread_scheduler()` function serves as the core, responsible for 
scheduling and switching between threads in the system. When yield is called,
the scheduler selects the next available thread to run from the ready queue,
prioritizing threads based whos scheduling policy, in this case 
priority-based scheduling. The function dequeues the selected thread from 
the ready queue, marks it as RUNNING, and enqueues the previously executing
thread back into the ready queue, ensuring fair execution among threads. 
The scheduler invokes the context switch operation to transition execution
from the current thread to the selected thread. This design allows for 
easy customization of scheduling policies and efficient management of concurrent execution.

The other functions implemented in `uthread.c`, such as `uthread_exit` and `uthread_block`,
are used for termination, and blocking. These functions leverage the underlying thread 
structures and queue operations to ensure proper execution of threads in the system. 
By having the thread management logic into reusable functions, uthread.c promotes ease of 
maintenance. lastly, the use of queues helped managed the ready threads which maked 
scheduling and execution of threads more effcient, enabling use to handle concurrent 
execution. 



## Semaphores
## Preemption
## Testing
## conclusion
