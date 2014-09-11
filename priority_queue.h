#ifndef PRIORITY_QUEUE_H_
#define PRIORITY_QUEUE_H_

#include "heap.h"

typedef struct _MaxPriQueue
{
    MaxHeap* heap;
} MaxPriQueue;

MaxHeapItem* GetPriQueueMax(MaxPriQueue* queue);
void PopPriQueueMax(MaxPriQueue* queue, MaxHeapItem* item);
void PushPriQueueMax(MaxPriQueue* queue, MaxHeapItem* item);

#endif