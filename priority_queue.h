#ifndef PRIORITY_QUEUE_H_
#define PRIORITY_QUEUE_H_

#include "heap.h"

typedef struct _MaxPriQueue
{
    Heap heap;
} MaxPriQueue;

void MaxPriQueueInit(MaxPriQueue* queue, int capacity);
HeapItem* GetPriQueueMax(MaxPriQueue* queue);
void MaxPriQueuePop(MaxPriQueue* queue, HeapItem* dst);
void MaxPriQueuePush(MaxPriQueue* queue, int key, void* data);
void MaxPriQueueRemove(MaxPriQueue* queue, int i, HeapItem* dst);

typedef struct _MinPriQueue
{
    Heap heap;
} MinPriQueue;

void MinPriQueueInit(MinPriQueue* queue, int capacity);
HeapItem* GetPriQueueMin(MinPriQueue* queue);
void MinPriQueuePop(MinPriQueue* queue, HeapItem* dst);
void MinPriQueuePush(MinPriQueue* queue, int key, void* data);
void MinPriQueueRemove(MinPriQueue* queue, int i, HeapItem* dst);

#endif