#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "pointqueue.h"

void Init_PointQueue(PointQueue *queue) {
	queue->head = NULL;
	queue->tail = NULL;
	queue->size = 0;
}

void Print_PointNode(PointQueue *queue) {
	int i;
	PointNode *node = queue->head;
	if (queue->size == 0)	return;

	for (i = 0; i < queue->size; i++) {
		printf("context : x = %d, y = %d\n", node->x, node->y);
		node = node->right;
	}
}

int Empty_PointQueue(PointQueue *queue) {
    if (queue->size > 0) {
    PointNode *old, *tmp = queue->head;
    while(tmp->right != NULL) {
        old = tmp;
        free(old);
        tmp = tmp->right;
    }
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    return 0;
    }
    else    return -1;
}

PointNode *Create_PointNode(int x, int y) {
	PointNode *node = (PointNode *)malloc(sizeof(PointNode));

	if (node == NULL)
		return node;

    node->x - x;
    node->y = y;

	node->left = NULL;
	node->right = NULL;
	return node;
}

int Enqueue_PointQueue(PointQueue *queue, int x, int y) {

	PointNode *node = Create_PointNode(x, y);

	if (node == NULL) {
		printf("failed create new node\n");
		return -1;
	}

	if (queue->head == NULL && queue->tail == NULL) {
		queue->head = node;
		queue->tail = node;
	}
	else {
		PointNode *end = queue->tail;
		node->left = end;
		end->right = node;
		queue->tail = node;
	}
	queue->size++;
	//printf("head : %x, tail : %x\n", queue->head, queue->tail);
	printf("Enqueue x = %d, y = %d\n", x, y);
	return 0;
}

int Dequeue_PointQueue(PointQueue *queue, int *x, int *y) {

	if (!Empty_Queue(queue)) {
		PointNode *tmp;
		PointNode *first = queue->head;
		queue->head = first->right;
		tmp = first->right;
		if (tmp != NULL)
			tmp->left = NULL;

        *x = first->x;
        *y = first->y;

		free(first);
		//printf("head : %x, tail : %x\n", queue->head, queue->tail);
		//printf("Dequeue %s\n", str);
		queue->size--;

		return 0;
	}
	else {
		printf("queue is empty\n");
		return -1;
	}
}

int Pop_PointQueue(PointQueue *queue, int *x, int *y) {

	if (!Empty_Queue(queue)) {
		PointNode *tmp;
		PointNode *end = queue->tail;
		queue->tail = end->left;
		tmp = end->left;
		if (tmp != NULL)
			tmp->right = NULL;

        *x = end->x;
        *y = end->y;

		free(end);
		//printf("head : %x, tail : %x\n", queue->head, queue->tail);
		//printf("Pop %s\n", str);
		queue->size--;

		return 0;
	}
	else {
		printf("queue is empty\n");
		return -1;
	}
}
int Push_PointQueue(PointQueue *queue, int x, int y) {
	return Enqueue_PointQueue(queue, x, y);
}

