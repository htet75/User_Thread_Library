# User-level thread library

## Introduction
This project aims to develop a user-level thread library for Linux, which will
allow for the creation and execution of concurrent threads. Through this
library, applications will be able to utilize the benefits of multithreading,
such as improved performance and resource management. The implementation will
follow existing lightweight thread libraries by providing round-robin
scheduling, preemption, and thread synchronization through semaphores. The
functionality of the queue is defined in the **queue.h** header.

## Queue Implementation
In order to make the queue have O(1) functionality for most of the operations,
we had to use a **singly linked list** data structure. Each element in this list
(or node) points to the next element in the list, and the head and end of the
list are kept track of by the queue object. With the exception of
`queue_iterate` and `queue_delete`, each function has O(1) time. 

### struct queue layout
Each node in the list contained a void pointer to the data and a pointer to the
next node. The main **queue** object stores the number of elements in the
*length* variable and pointers to the *head* and *end* of the linked list,
labeled respectively.

### Queue Functions
The basic functionality of the queue is when we enqueue an item, we use
`queue_enqueue` which creates a new queue_node with the given data, adding the
object to the end of the linked list and updates it accordingly. When we want to
get an item from the queue, we use the `queue_dequeue`, which removes the item
from the head of the linked list and updates it accordingly. `queue_create` does
all the necessary initialization of the `queue_t` object. To delete specific
elements in the queue, we use `queue_delete` which iterates through the list and
if given the data, we remove it. For freeing memory, we use `queue_destroy`,
which removes the queue object itself if empty. One of the unique functions of
our queue is `queue_iterate` which goes through the linked list of data, and
executes a function on every item enqueued in the queue.

### Queue Testing
Intial testing was done using the queue_tester_example.x found in apps.
Afterwards, examples were programmed from the given prompt to ensure proper
behavior to the given specifications. Afterwards, error testing the edge cases
with the queue was done alongside our own creations of example apps using the
queue. 

## User Thread Implementation
The goal was to make a thread library that used Round Robin Scheduling in order
to ensure fairness in time execution for each thread. In order to keep track of
all the threads and integrate round robin scheduling, a queue named
`thread_queue` was used to rotate through the executing threads and the current
running thread was stored in a pointer named `current_thread`. The library first
gets intialized by `uthread_run` and creates the main thread which will
continously yield until all threads are finished running. The functionality of
the user thread library is defined in the **uthread.h** header.

### User Thread object
There were various private data structures that were created to deal with the
creation and scheduling of the threads. Every user thread has a `uthread_tcb`
containing the *context*, *stack*, and *state* of the associated thread. The
*context* and *stack* are created from the **context.c** file. The different
states are taken from the `uthread_state_t` enumerator which stores the 4
possible states for threads to be in, **READY**, **RUNNING**, **ZOMBIE**, and
**BLOCKED**. 

### User Thread functions
`uthread_run` is the **first** function that should be ran to use our user threaded
library. Properly initializes the main thread and creates the first running
thread and associated thread_queue, and handling preemption if desired.

`uthread_yield` allows for the oldest thread in the `thread_queue` to run and
handles the context switching. Also it handles freeing up any thread that is a
zombie.

`uthread_create` allows for the creation of another thread and enqueues it onto
the `thread_queue` to begin the round robin scheduling.

`helper_thread_creator` is a private function that is used to create a thread,
intializing the thread alongside the context and stack.

`uthread_exit` is what a user thread calls when it finishes the execution. It
partially destroys itself but hands it off to yield to finish the rest of the
memory freeing.

`uthread_(block/unblock)` are private functions that change the state of the
thread, and when unblocked, enqueues the thread back into the ready queue so
that it can begin execution again.

### User Thread Testing
The user thread were mainly tested using the **uthread_(hello/yield).x** apps.
Custom apps were also created to further test the round_robin behavior of the
app.

## Semaphore Implementation
Semaphores are useful to regulate access to common resources when using multiple
threads. We created a private data structure and leveraged a queue in order to
have an connected way to access these values and store the list of threads
that want to utilize the semaphore resource.

### Semaphore objects
`struct semaphore` is the private data structure created by us to represent and
maintain the *internal count* for all the various threads accessing it. Each
semaphore also has an associated *waiting_list*, which is a queue that keeps the
list of threads that wish to access it. 

### Semaphore functions
`sem_create` is the initial function used to create a semaphore object and
properly initializes the count and queue of the semaphore.

`sem_destroy` attempts to destroy the queue and if it works, frees the semaphore
object itself.

`sem_(up/down)` are implementing the basic operations of a semaphore. `sem_down`
handles decreasing the semaphore's internal count and enqueues threads to
*waiting_list* queue if needed. `sem_up` increases the internal count if
there are no threads in the *waiting_list* queue, otherwise it dequeues and
unblocks that given thread to be used later on.

### Semaphore testing
Initial testing was done using the **sem_(simple/count/buffer/prime).x** apps
alongside small testing cases to ensure the internal count and queues associated
with the semaphores were properly functioning.

## Preemption Implementation
Preemption allows our library to take control of any thread without requiring
the user to yield. This is mainly done by leveraging the **SIGVTALRM** signal
and setting up an internal timer to trigger the signal at a rate of 100 Hz.
Everything the signal is called, the currently running thread is yielded to
allow for the next oldest thread in the `thread_queue` to run. The library will
store the previous action and timers of the associated signal before running and
will restore the configurations after the library exits.

### Preemption changes in other files
The difficulty of implementing preemption was deciding what parts of our user
threads and semaphores were critical sections that we wanted to function
atomically so we would had to disable the preemption. This mainly affected
sections where there would be functions that would potentially cause issues when
running between multiple threads such as functions relating to the semaphores or
parts of the user threads where we had to use the `thread_queue`.

### Preemption functions
`preempt_start` starts up our preemption and initializes the signal and internal
timer that keeps track of the interrupts in our applications. It uses the
**SIGVTALRM** signal and connected `timer_interrupt_handler` to run when the
signal is called. Next it setups the **ITIMER_VIRTUAL** to handle the correct
timing of 100 Hz and fires an alarm when the time is up.

`preempt_(enable/disable)` are our main functions to create critical sections of
code that can't be interrupted by preemption. This is especially important to
use when executing code that shares memory such as our `thread_queue`. Both
execute in a similar fashion, creating a signal mask that either blocks or
unblocks the **SIGVTALRM** signal. 

`preempt_stop` is called after the user threaded library finishes, which simply
restores the previous actions and timer configuration for the **SIGVTALRM** and
**ITIMER_VIRTUAL**. 

`timer_interrupt_handler` is our function that is called whenever **SIGVTALRM**
is called, and simplies yields whatever is the current running thread.

### Preemption Testing
We created our own program which would have threads that run a loop that takes a
significant amount of time printing whatever thread it is in. For us to visually
inspect preemption, we see that the first thread isn't able to finish its loop
before another thread prints out their input. If preemption wasn't enabled, the
thread would print out 10 rows of "From Thread 1" and then 10 rows of "From
Thread 2", and then 10 rows of "From Thread 3". Instead we see that the threads
themselves aren't able to fully execute despite having no yields, instead
printing out "From Thread 1" and then moving to potentially "From Thread 2" or
"From Thread 3".