#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "pointQueue.h"

void Init_Queue(pointQueue *queue) {
	queue->head = NULL;
	queue->tail = NULL;
	queue->size = 0;
}

void Print_Node(pointQueue *queue) {
	int i;
	Node *node = queue->head;
	if (queue->size == 0)	return;

	for (i = 0; i < queue->size; i++) {
		printf("context : x = %d, y = %d\n", node->x, node->y, node->context);
		node = node->right;
	}
}

int Empty_Queue(pointQueue *queue) {
	if (queue->size > 0)
		return 0;
	else	return -1;
}

pointNode *Create_Node(int x, int y) {
	Node *node = (Node *)malloc(sizeof(Node));

	if (node == NULL)
		return node;

    node->x - x;
    node->y = y;

	node->left = NULL;
	node->right = NULL;
	return node;
}

int Enqueue(pointQueue *queue, int x, int y) {

	pointNode *node = Create_Node(x, y);

	if (node == NULL) {
		printf("failed create new node\n");
		return -1;
	}

	if (queue->head == NULL && queue->tail == NULL) {
		queue->head = node;
		queue->tail = node;
	}
	else {
		Node *end = queue->tail;
		node->left = end;
		end->right = node;
		queue->tail = node;
	}
	queue->size++;
	//printf("head : %x, tail : %x\n", queue->head, queue->tail);
	printf("Enqueue x = %d, y = %d\n", x, y);
	return 0;
}

int *Dequeue(pointQueue *queue, int *x, int *y) {

	if (!Empty_Queue(queue)) {
		pointNode *tmp;
		pointNode *first = queue->head;
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

int *Pop(Queue *queue, int *x, int *y) {

	if (!Empty_Queue(queue)) {
		Node *tmp;
		Node *end = queue->tail;
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
int Push(pointQueue *queue, int x, int y) {
	return Enqueue(queue, int x, int y);
}

