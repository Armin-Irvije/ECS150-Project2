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
## Semaphores
## Preemption
## Testing
## conclusion
