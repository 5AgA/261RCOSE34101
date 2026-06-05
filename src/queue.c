#include <stddef.h>
#include "../include/queue.h"

void queue_init(Queue *q) {
    q->front = 0;
    q->rear  = -1;
    q->size  = 0;
}

int queue_empty(Queue *q) { return q->size == 0; }
int queue_full (Queue *q) { return q->size == MAX_PROCESSES; }

// 뒤에 추가
void enqueue(Queue *q, Process *p) {
    if (queue_full(q)) return;
    q->rear = (q->rear + 1) % MAX_PROCESSES;
    q->data[q->rear] = p;
    q->size++;
}

// 앞에서 제거
Process *dequeue(Queue *q) {
    if (queue_empty(q)) return NULL;
    Process *p = q->data[q->front];
    q->front = (q->front + 1) % MAX_PROCESSES;
    q->size--;
    return p;
}

// 맨 앞 큐 읽어오기
Process *queue_peek(Queue *q) {
    if (queue_empty(q)) return NULL;
    return q->data[q->front];
}
