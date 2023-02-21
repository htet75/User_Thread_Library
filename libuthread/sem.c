#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	/* Data structure for the semaphore */
	size_t internal_count;
	queue_t waiting_list;
};

sem_t sem_create(size_t count)
{
	preempt_disable();
	/* Creating a semaphore object */
	sem_t new_sem = (sem_t)malloc(sizeof(struct semaphore));
	if (new_sem == NULL)
		return NULL;

	/* Initialize variables */
	new_sem->internal_count = count;
	new_sem->waiting_list = queue_create();
	preempt_enable();

	return new_sem;
}

int sem_destroy(sem_t sem)
{
	preempt_disable();
	/* Remove the semaphore */
	if (sem == NULL || queue_destroy(sem->waiting_list) == -1)
		/* Check if semaphore is a NULL pointer or it still has queues blocking */
		return -1;

	free(sem);

	preempt_enable();
	return 0;
}

int sem_down(sem_t sem)
{
	preempt_disable();
	/* Check if there is a NULL pointer */
	if (sem == NULL)
		return -1;

	
	/* Decrement the internal count in the semaphore and
	 * if there is no more possible resources, remove the
	 * add the semaphores to the queue
	 */
	if (sem->internal_count > 0)
	{
		sem->internal_count--;
	}
	else
	{
		struct uthread_tcb *waitlisted_thread = uthread_current();
		
		queue_enqueue(sem->waiting_list, waitlisted_thread);
		uthread_block();
	}
	preempt_enable();
	
	return 0;
}

int sem_up(sem_t sem)
{
	preempt_disable();
	/* Release the semaphore */
	if (sem == NULL)
		/* Check for NULL pointer */
		return -1;

	/* Check if there are any in waiting list */
	if (queue_length(sem->waiting_list) == 0)
	{
		sem->internal_count++;
	}
	else
	{
		struct uthread_tcb *blocked_thread;
		if (queue_dequeue(sem->waiting_list, (void**) &blocked_thread) == -1)
			return -1;

		uthread_unblock(blocked_thread);
	}

	preempt_enable();
	return 0;
}

