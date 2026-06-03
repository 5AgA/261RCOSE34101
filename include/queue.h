#ifndef QUEUE_H
#define QUEUE_H

#include "types.h"

typedef struct {
    Process *data[MAX_PROCESSES];
    int front, rear, size;
} Queue;

void    queue_init  (Queue *q);
int     queue_empty (Queue *q);
int     queue_full  (Queue *q);
void    enqueue     (Queue *q, Process *p);
Process *dequeue    (Queue *q);
Process *queue_peek (Queue *q);

#endif
