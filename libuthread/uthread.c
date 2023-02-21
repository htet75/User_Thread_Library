#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "queue.h"
#include "uthread.h"

/* Main queue to keep track of all threads in a Round Robin Scheduling */
queue_t thread_queue;

/* Pointer to keep track of the currently executing thread */
struct uthread_tcb *current_thread; 

/* Different states the processor can be in */
enum uthread_state_t
{
	READY,
	RUNNING,
	ZOMBIE,
	BLOCKED
};

struct uthread_tcb {
	/* Thread control block containing information about the thread */
	uthread_ctx_t *context;
	void *stack;
	enum uthread_state_t state;
};

struct uthread_tcb *uthread_current(void)
{
	/* Returns a pointer to the current thread */
	return current_thread;
}

void uthread_yield(void)
{
	/* Give another thread a chance to run */
	struct uthread_tcb *saved_thread = uthread_current();
	struct uthread_tcb *new_thread;

	/* Prepare save thread to switch if not terminated */
	
	if (saved_thread->state == RUNNING)
	{
		/* Check if the thread is running to stop it */
		saved_thread->state = READY; // Prepare thread to be switched
		preempt_disable();
		queue_enqueue(thread_queue, saved_thread); // Add the original thread into queue
		preempt_enable();
	}
	
	// Prepare new thread to run
	preempt_disable();
	assert(queue_dequeue(thread_queue, (void**)&new_thread) == 0); // Check if failed to get element from queue (since it has no return checking)
	preempt_enable();
	new_thread->state = RUNNING;
	current_thread = new_thread; // Update thread

	uthread_ctx_switch(saved_thread->context, new_thread->context); // Switch context with another context

	/* Free up any thread that is already terminated */
	if (saved_thread->state == ZOMBIE)
	{
		uthread_ctx_destroy_stack(saved_thread->stack);
		free(saved_thread->context);
		free(saved_thread);
		saved_thread = NULL;
	}
}

void uthread_exit(void)
{
	/* Finish exiting a thread */
	struct uthread_tcb *exiting_thread = uthread_current();

	/* Free memory of thread information */
	exiting_thread->state = ZOMBIE;
	uthread_ctx_destroy_stack(exiting_thread->stack);

	/* Deallocating TCB memory */
	// free(exiting_thread);
	
	/* Pass to yield to handle switching to next thread */
	uthread_yield(); // Never returns so yield will remove the thread
}

struct uthread_tcb *helper_uthread_creator()
{
	/* Creates some elements of a thread that don't require any input
	   Returns a NULL if any function fails
	 */
	/* Creates a thread by initializing the TCB */
	struct uthread_tcb *uthread = (struct uthread_tcb *)malloc(sizeof(struct uthread_tcb));
	if (uthread == NULL)
		/* Failed to malloc tcb*/
		return NULL;
	
	/* Allocate context for thread */
	uthread->context = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
	if (uthread->context == NULL)
	{
		/* Failed to malloc context */
		free(uthread);
		return NULL;
	}


	uthread->state = READY;
	
	/* Allocate Stack for thread*/
	uthread->stack = uthread_ctx_alloc_stack();
	if (uthread->stack == NULL)
	{
		/* Failed to get stack pointer */
		free(uthread->context);
		free(uthread);
		return NULL;
	}

	return uthread;
}

int uthread_create(uthread_func_t func, void *arg)
{
	/* Creates a new thread*/
	struct uthread_tcb *uthread = helper_uthread_creator();
	if (uthread == NULL)
		return -1;

	if (uthread_ctx_init(uthread->context, uthread->stack, func, arg) == -1)
		/* Context creation failed */
		return -1;
	preempt_disable();
	queue_enqueue(thread_queue, (void*)uthread); // Add new thread to the queue
	preempt_enable();

	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
    /* First function to initialize the uthread library and create a thread */
    thread_queue = queue_create();
    if (thread_queue == NULL)
    {
        /* Unable to create queue */
        return -1;
    }

    /* Main idle thread */
    struct uthread_tcb* main_idle_thread = helper_uthread_creator();
    if (main_idle_thread == NULL)
        /* Unable to create thread */
        return -1;
    
    main_idle_thread->state = RUNNING;

    if (uthread_create(func, arg) == -1)
        /* Create the first thread */
        return -1;

    current_thread = main_idle_thread; // Set current thread to be the first idle thread
    
    preempt_start(preempt);
	// while(1);
    while (queue_length(thread_queue) != 0)
    {
        /* Idle loop while the queue is not empty */
        uthread_yield();
    }

    preempt_stop();

    return 0;
}

void uthread_block(void)
{
	/* Block current thread and yield for another thread in queue */
	uthread_current()->state = BLOCKED;
	uthread_yield();
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* Unblock the thread and push it back into the ready queue */
	uthread->state = READY;
	preempt_disable();
	queue_enqueue(thread_queue, uthread);
	preempt_enable();
}

