#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "doublequeue.h"

void Init_DoubleQueue(DoubleQueue *queue) {
	queue->head = NULL;
	queue->tail = NULL;
	queue->size = 0;
}

void Print_DoubleNode(DoubleQueue *queue) {
	int i;
	DoubleNode *node = queue->head;
	if (queue->size == 0)	return;

	for (i = 0; i < queue->size; i++) {
		printf("value : %lf\n", node->value);
		node = node->right;
	}
}
void Clear_DoubleQueue(DoubleQueue *queue) {
    DoubleNode *old, *tmp = queue->head;
    while(tmp != NULL) {
        old = tmp;
        free(old);
        tmp = tmp->right;
    }
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
}
int Is_Empty_DoubleQueue(DoubleQueue *queue) {
    if (queue->size > 0) {
        return 0;
    }
    else return -1;
}

DoubleNode *Create_DoubleNode(double value) {
	DoubleNode *node = (DoubleNode *)malloc(sizeof(DoubleNode));

	if (node == NULL)
		return node;

    node->value = value;

	node->left = NULL;
	node->right = NULL;
	return node;
}

int Enqueue_DoubleQueue(DoubleQueue *queue, double value) {

	DoubleNode *node = Create_DoubleNode(value);

	if (node == NULL) {
		printf("failed create new node\n");
		return -1;
	}

	if (queue->head == NULL && queue->tail == NULL) {
		queue->head = node;
		queue->tail = node;
	}
	else {
		DoubleNode *end = queue->tail;
		node->left = end;
		end->right = node;
		queue->tail = node;
	}
	queue->size++;
	//printf("head : %x, tail : %x\n", queue->head, queue->tail);
    //printf("Enqueue x = %d, y = %d\n", x, y);
	return 0;
}

double Dequeue_DoubleQueue(DoubleQueue *queue) {

    double value;
	if (!Is_Empty_Queue(queue)) {
		DoubleNode *tmp;
		DoubleNode *first = queue->head;
		queue->head = first->right;
		tmp = first->right;
		if (tmp != NULL)
			tmp->left = NULL;
        value = first->value;
		free(first);
		//printf("head : %x, tail : %x\n", queue->head, queue->tail);
		//printf("Dequeue %s\n", str);
		queue->size--;
        if(queue->size == 0)
        {
            queue->head = NULL;
            queue->tail = NULL;
        }

		return value;
	}
	else {
		printf("queue is empty\n");
		return -1;
	}
}

double Pop_DoubleQueue(DoubleQueue *queue) {

    double value;
	if (!Is_Empty_Queue(queue)) {
		DoubleNode *tmp;
		DoubleNode *end = queue->tail;
		queue->tail = end->left;
		tmp = end->left;
		if (tmp != NULL)
			tmp->right = NULL;

        value = end->value;
		free(end);
		//printf("head : %x, tail : %x\n", queue->head, queue->tail);
		//printf("Pop %s\n", str);
		queue->size--;
        if(queue->size == 0)
        {
            queue->head = NULL;
            queue->tail = NULL;
        }

		return value;
	}
	else {
		printf("queue is empty\n");
		return -1;
	}
}
int Push_DoubleQueue(DoubleQueue *queue, double value) {
	return Enqueue_DoubleQueue(queue, value);
}
A

