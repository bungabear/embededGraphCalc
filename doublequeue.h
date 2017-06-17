#ifndef DOUBLEQUEUE_H
#define DOUBLEQUEUE_H
typedef struct _DoubleNode {
    double value;
	struct _DoubleNode *left, *right;
}DoubleNode;

typedef struct {
	DoubleNode *head, *tail;
	int size;
}DoubleQueue;

DoubleNode *Create_DoubleNode(double value);
void Init_DoubleQueue(DoubleQueue *queue);
void Print_DoubleNode(DoubleQueue *queue);
void Clear_DoubleQueue(DoubleQueue *queue);
int Is_Empty_DoubleQueue(DoubleQueue *queue);
int Enqueue_DoubleQueue(DoubleQueue *queue, double value);
double Dequeue_DoubleQueue(DoubleQueue *queue);
double Pop_DoubleQueue(DoubleQueue *queue);
int Push_DoubleQueue(DoubleQueue *queue, double value);
#endif
