#ifndef QUEUE_H
#define QUEUE_H
typedef struct _node {
	char *context;
	struct _node *left, *right;
}Node;

typedef struct {
	Node *head, *tail;
	int size;
}Queue;

Node *Create_Node(char *str);
void Print_Node(Queue *queue);
void Init_Queue(Queue *queue);
int Empty_Queue(Queue *queue);
int Enqueue(Queue *queue, char *str);
char *Dequeue(Queue *queue);
char *Pop(Queue *queue);
int Push(Queue *queue, char *str);
#endif
