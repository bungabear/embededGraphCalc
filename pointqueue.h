#ifndef POINTQUEUE_H
#define POINTQUEUE_H
typedef struct _PointNode {
    int x;
    int y;
	struct _PointNode *left, *right;
}PointNode;

typedef struct {
	PointNode *head, *tail;
	int size;
}PointQueue;

PointNode *Create_PointNode(int x, int y);
void Init_PointQueue(PointQueue *queue);
void Print_PointNode(PointQueue *queue);
void Clear_PointQueue(PointQueue *queue);
int Is_Empty_PointQueue(PointQueue *queue);
int Enqueue_PointQueue(PointQueue *queue, int x, int y);
int Dequeue_PointQueue(PointQueue *queue, int *x, int *y);
int Pop_PointQueue(PointQueue *queue, int *x, int *y);
int Push_PointQueue(PointQueue *queue, int x, int y);
#endif
