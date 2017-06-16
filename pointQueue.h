typedef struct _node {
    int x;
    int y;
	struct _node *left, *right;
}pointNode;

typedef struct {
	pointNode *head, *tail;
	int size;
}pointQueue;

pointNode *Create_Node(int x, int y);
void Init_Queue(pointQueue *queue);
void Print_Node(pointQueue *queue);
int Empty_Queue(pointQueue *queue);
int Enqueue(pointQueue *queue, int x, int y);
int *Dequeue(pointQueue *queue, int *x, int *y);
int *Pop(pointQueue *queue, int *x int *y);
int Push(pointQueue *queue, int x, int y);
