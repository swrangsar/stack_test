#include "queue.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "logmsg.h"



static void queue_clear(struct Queue *queue);

bool queue_init(DestroyFunc destroy_func, struct Queue *queue)
{
	bool is_valid = false;

	if (queue) {
		queue->head = NULL;
		queue->tail = NULL;
		queue->destroy_func = destroy_func;
		is_valid = true;
	} else {
		log_err("init null queue\n");
	}

	return is_valid;
}

int enqueue(struct Queue *queue, void *data)
{
	int ret_code = -1;
	struct node *node;

	if (queue) {
		if ((node = malloc(sizeof(*node)))) {
			node->data = data;
			node->next = NULL;
			if (queue->tail) {
				queue->tail->next = node;
				queue->tail = node;
			} else {
				queue->tail = node;
				queue->head = node;
			}

			ret_code = 0;
		} else {
			log_err("enqueue: malloc node");
		}
	} else {
		log_err("enqueue: null queue\n");
	}

	return ret_code;
}

bool is_queue_empty(struct Queue *queue)
{
	bool is_empty = true;

	if (queue) {
		is_empty = !queue->head;
	} else {
		log_err("null queue\n");
	}

	return is_empty;
}

void *dequeue(struct Queue *queue)
{
	struct node *node;
	void *data = NULL;

	if (queue) {
		if ((node = queue->head)) {
			data = node->data;
			queue->head = node->next;
			if (!queue->head)
				queue->tail = NULL;

			free(node);
		}
	} else {
		log_err("dequeue: null queue\n");
	}

	return data;
}

static void queue_clear(struct Queue *queue)
{
	if (!queue)
		return;

	if (queue->destroy_func) {
		while (!is_queue_empty(queue))
			queue->destroy_func(dequeue(queue));
	} else {
		while (!is_queue_empty(queue))
			dequeue(queue);
	}
}

void queue_destroy(struct Queue *queue)
{
	if (queue) {
		queue_clear(queue);
	} else {
		log_err("can't destroy null queue\n");
	}
}
