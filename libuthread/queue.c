#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct queue_node {
	/* Linked list node to store queue data */
	void *data;
	struct queue_node *next;
};

struct queue {
	/* Main queue object that stores the head and end node of the linked list */
	struct queue_node *head;
	struct queue_node *end;
	int length;
};

queue_t queue_create(void)
{
	/* Creates a queue object */
	struct queue* q = (struct queue*)malloc(sizeof(struct queue));
	if (!q)
	{
		return NULL;
	}
	// Initialize values for queue
	q->head = NULL;
	q->end = NULL;
	q->length = 0;
	
	return q;
}

int queue_destroy(queue_t queue)
{
	/* Removes a queue object from memory */
	if (!queue || queue->length != 0)
		/* Checking for NULL pointer and checking if the queue is empty */
		return -1;

	free(queue);

	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	/* Add a queue element to data */
	if(queue == NULL || data == NULL)
		return -1;

	struct queue_node *new_node = (struct queue_node*)malloc(sizeof(struct queue_node));
	if (new_node == NULL)
		return -1;

	new_node->data = data;
	new_node->next = NULL;

	if (queue->head == NULL)
	{
		/* Update the queue to add the first element */
		queue->head = new_node;
		queue->end = new_node;
	}
	else
	{
		/* Update the end of the linked list with a new node */
		queue->end->next = new_node;
		queue->end = new_node;
	}

	queue->length++;
	
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	/* Remove a queue element */
	if (queue == NULL || data == NULL || queue->length == 0)
		/* Checking for invalid input */
		return -1;
	
	*data = (queue->head->data);
	struct queue_node *delete_node = queue->head;
	queue->head = queue->head->next;
	if (queue->head == NULL)
		/* Check if it was the last element in the list*/
		queue->end = NULL;

	free(delete_node);
	queue->length--;

	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	/* Delete an element from queue */
	if (queue == NULL || data == NULL)
		return -1;
	
	struct queue_node *current_node = queue->head;
	struct queue_node *previous_node = queue->head;
	while (current_node)
	{
		if (current_node->data == data)
		{
			// Found the data in the node
			if (current_node == queue->head)
			{
				// Data removed is first element in linked list
				queue->head = queue->head->next;
				if (queue->head == NULL)
					/* Check if it was the last element in the list*/
					queue->end = NULL;
			}
			else
			{
				// Removed data in linked list
				previous_node->next = current_node->next;
			}
			queue->length--;
			free(current_node);

			return 0;
		}

		previous_node = current_node;
		current_node = current_node->next;
	}

	return -1;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	/* Call a custom function on every item enqueued in the queue */
	if (queue == NULL || func == NULL)
		return -1;
	
	struct queue_node *current_node = queue->head;
	while(current_node != NULL)
	{
		void *node_data = current_node->data;
		current_node = current_node->next;
		func(queue, node_data);
	}

	return 0;
}

int queue_length(queue_t queue)
{
	/* Returns the length of the queue */
	if (queue == NULL)
		return -1;

	return queue->length;
}

