#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>


#define log_err(M)	{perror("error: queue: " M); goto error;}
#define log_msg(M)	{fprintf(stderr, "error: queue: " M "\n"); goto error;}

typedef struct _Node Node;

static void queue_clear(Queue *queue);



struct _Node {
	Node *next;
	void *data;
};

struct _Queue {
	Node *head;
	Node *tail;
	DestroyFunc destroy_func;
};


Queue *queue_new(DestroyFunc destroy_func)
{
	Queue *queue;
	
	if (!(queue = malloc(sizeof(*queue))))
		log_err("queue_new");

	queue->head = NULL;
	queue->tail = NULL;
	queue->destroy_func = destroy_func;
error:
	return queue;
}

int enqueue(Queue *queue, void *data)
{
	Node *node;
	
	if (!queue)
		log_msg("enqueue: null queue!");
	if (!(node = malloc(sizeof(*node))))
		log_err("enqueue: malloc node");

	node->data = data;
	node->next = queue->head;
	queue->head = node;
	
	if (!node->next)
		queue->tail = node;
	
	return 0;
error:
	return -1;
}

int queue_is_empty(Queue *queue)
{
	if (!queue)
		log_msg("queue_is_empty: null queue!");

	return !queue->head;
error:
	return 1;
}

void *dequeue(Queue *queue)
{
	Node *node;
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

static void queue_clear(Queue *queue)
{
	void *data;

	if (!queue)
		return;

	if (queue->destroy_func) {
		while (!queue_is_empty(queue))
			queue->destroy_func(data = dequeue(queue));
	} else {
		while (!queue_is_empty(queue))
			dequeue(queue);
	}
}

void queue_destroy(Queue *queue)
{
	if (!queue)
		return;

	queue_clear(queue);
	free(queue);
}
