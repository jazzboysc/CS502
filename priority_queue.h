#ifndef PRIORITY_QUEUE_H_
#define PRIORITY_QUEUE_H_

#include "heap.h"

typedef struct _MaxPriQueue
{
    Heap* heap;
} MaxPriQueue;

HeapItem* GetPriQueueMax(MaxPriQueue* queue);
void PopPriQueueMax(MaxPriQueue* queue, HeapItem* item);
void PushPriQueueMax(MaxPriQueue* queue, HeapItem* item);

#endif