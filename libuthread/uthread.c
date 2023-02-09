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

queue_t thread_queue;
struct uthread_tcb *current_thread;

enum uthread_state_t
{
	READY,
	RUNNING,
	EXITED
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
		queue_enqueue(thread_queue, saved_thread); // Add the original thread into queue
	}
	
	// Prepare new thread to run
	assert(queue_dequeue(thread_queue, (void**)&new_thread) == 0); // Check if fialed to get element from queue
	new_thread->state = RUNNING;
	current_thread = new_thread; // Update thread

	uthread_ctx_switch(saved_thread->context, new_thread->context); // Switch context with another context

	if (saved_thread->state == EXITED)
	{
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
	exiting_thread->state = EXITED;
	uthread_ctx_destroy_stack(exiting_thread->stack);
	
	/* Pass to yield to handle switching to next thread */
	uthread_yield(); // Never returns so yield will remove the thread
}

int uthread_create(uthread_func_t func, void *arg)
{
	/* Creates a thread by initializing the TCB */
	struct uthread_tcb *uthread = (struct uthread_tcb *)malloc(sizeof(struct uthread_tcb));
	if (uthread == NULL)
		/* Failed to malloc tcb*/
		return -1;
	
	/* Allocate context for thread */
	uthread->context = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
	if (uthread->context == NULL)
		/* Failed to malloc context */
		return -1;

	uthread->state = READY;
	
	/* Allocate Stack for thread*/
	uthread->stack = uthread_ctx_alloc_stack();
	if (uthread->stack == NULL)
		/* Failed to get stack pointer */
		return -1; 

	if (uthread_ctx_init(uthread->context, uthread->stack, func, arg) == -1)
		/* Context creation failed */
		return -1;

	queue_enqueue(thread_queue, (void*)uthread); // Add new thread to the queue
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
	struct uthread_tcb* main_idle_thread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	if (main_idle_thread == NULL)
		/* Unable to create thread */
		return -1;
	
	/* Allocate context for main idle thread */
	main_idle_thread->context = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
	if (main_idle_thread->context == NULL)
		/* Unable to malloc context */
		return -1;

	main_idle_thread->state = RUNNING;

	/* Allocate stack for main idle thread */
	main_idle_thread->stack = uthread_ctx_alloc_stack();
	if (main_idle_thread->stack == NULL)
		/* Failed to get stack pointer */
		return -1; 

	if (uthread_create(func, arg) == -1)
		/* Create the first thread */
		return -1;

	current_thread = main_idle_thread; // Set current thread

	while (queue_length(thread_queue) != 0)
		/* Idle loop while the queue is not empty */
		uthread_yield();

	return 0;
}

void uthread_block(void)
{
	/* TODO Phase 3 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
}

