#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "queue.h"

void Init_Queue(Queue *queue) {
	queue->head = NULL;
	queue->tail = NULL;
	queue->size = 0;
}

void Print_Node(Queue *queue) {
	int i;
	Node *node = queue->head;
	if (queue->size == 0)	return;

	for (i = 0; i < queue->size; i++) {
		printf("left : %x, right : %x, context : %s\n", node->left, node->right, node->context);
		node = node->right;
	}
}

int Empty_Queue(Queue *queue) {
	if (queue->size > 0)
		return 0;
	else	return -1;
}

Node *Create_Node(char *str) {
	Node *node = (Node *)malloc(sizeof(Node));
	int len;

	if (node == NULL)
		return node;

	len = strlen(str);
	node->context = (char *)malloc(sizeof(char)*(len + 1));
	strncpy(node->context, str, len + 1);

	node->left = NULL;
	node->right = NULL;
	return node;
}

int Enqueue(Queue *queue, char *str) {

	Node *node = Create_Node(str);

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
	printf("Enqueue %s\n", str);
	return 0;
}

char *Dequeue(Queue *queue) {
	char *str = NULL;
	int len;

	if (!Empty_Queue(queue)) {
		Node *tmp;
		Node *first = queue->head;
		queue->head = first->right;
		tmp = first->right;
		if (tmp != NULL)
			tmp->left = NULL;

		len = strlen(first->context);
		str = (char *)malloc(sizeof(char)*(len + 1));
		strncpy(str, first->context, len + 1);

		free(first);
		//printf("head : %x, tail : %x\n", queue->head, queue->tail);
		//printf("Dequeue %s\n", str);
		queue->size--;

		return str;
	}
	else {
		printf("queue is empty\n");
		return str;
	}
}

char *Pop(Queue *queue) {
	char *str = NULL;
	int len;

	if (!Empty_Queue(queue)) {
		Node *tmp;
		Node *end = queue->tail;
		queue->tail = end->left;
		tmp = end->left;
		if (tmp != NULL)
			tmp->right = NULL;

		len = strlen(end->context);
		str = (char *)malloc(sizeof(char)*(len + 1));
		strncpy(str, end->context, len + 1);

		free(end);
		//printf("head : %x, tail : %x\n", queue->head, queue->tail);
		//printf("Pop %s\n", str);
		queue->size--;

		return str;
	}
	else {
		printf("queue is empty\n");
		return str;
	}
}
int Push(Queue *queue, char *str) {
	return Enqueue(queue, str);
}

