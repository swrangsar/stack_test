#include "queue.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>


#define log_err(M)	{perror("error: queue: " M); goto error;}
#define log_msg(M)	{fprintf(stderr, "error: queue: " M "\n"); goto error;}


static void queue_clear(struct Queue *queue);





struct Queue *queue_new(DestroyFunc destroy_func)
{
	struct Queue *queue;
	
	if (!(queue = malloc(sizeof(*queue))))
		log_err("queue_new");

	queue->head = NULL;
	queue->tail = NULL;
	queue->destroy_func = destroy_func;
error:
	return queue;
}

int enqueue(struct Queue *queue, void *data)
{
	struct node *node;
	
	if (!queue)
		log_msg("enqueue: null queue!");
	if (!(node = malloc(sizeof(*node))))
		log_err("enqueue: malloc node");

	node->data = data;
	node->next = NULL;
	if (queue->tail) {
		queue->tail->next = node;
		queue->tail = node;
	} else {
		queue->tail = node;
		queue->head = node;
	}
	
	return 0;
error:
	return -1;
}

int queue_is_empty(struct Queue *queue)
{
	if (!queue)
		log_msg("queue_is_empty: null queue!");

	return !queue->head;
error:
	return 1;
}

void *dequeue(struct Queue *queue)
{
	struct node *node;
	void *data;
	
	if (!queue)
		log_msg("dequeue: null queue!");
	if (!(node = queue->head))
		return NULL;

	data = node->data;
	queue->head = node->next;
	if (!queue->head)
		queue->tail = NULL;

	free(node);
	return data;
error:
	return NULL;
}

static void queue_clear(struct Queue *queue)
{
	if (!queue)
		return;

	if (queue->destroy_func) {
		while (!queue_is_empty(queue))
			queue->destroy_func(dequeue(queue));
	} else {
		while (!queue_is_empty(queue))
			dequeue(queue);
	}
}

void queue_destroy(struct Queue *queue)
{
	if (!queue)
		return;

	queue_clear(queue);
	free(queue);
}
