#include "priority_queue.h"
#include <assert.h>

static int MaxPriQueueCompare(HeapItemKey* key1, HeapItemKey* key2)
{
    if( key1->key > key2->key )
    {
        return 1;
    }
    else if( key1->key < key2->key )
    {
        return -1;
    }
    else
    {
        if( key1->timeStamp < key2->timeStamp )
        {
            return 1;
        }
        else if( key1->timeStamp > key2->timeStamp )
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
}

static int MinPriQueueCompare(HeapItemKey* key1, HeapItemKey* key2)
{
    if( key1->key < key2->key )
    {
        return 1;
    }
    else if( key1->key > key2->key )
    {
        return -1;
    }
    else
    {
        if( key1->timeStamp < key2->timeStamp )
        {
            return 1;
        }
        else if( key1->timeStamp > key2->timeStamp )
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
}

//****************************************************************************
void MaxPriQueueInit(MaxPriQueue* queue, int capacity)
{
    assert( queue && capacity > 0 );
    HeapInit(&(*queue).heap, capacity, MaxPriQueueCompare, 1);
}
//****************************************************************************
HeapItem* GetPriQueueMax(MaxPriQueue* queue)
{
    assert( queue );
    assert( queue->heap.size >= 1 );

    return &queue->heap.buffer[1];
}
//****************************************************************************
void MaxPriQueuePop(MaxPriQueue* queue, HeapItem* dst)
{
    assert( queue && dst );
    HeapPop(&queue->heap, dst);
}
//****************************************************************************
void MaxPriQueuePush(MaxPriQueue* queue, int key, void* data)
{
    assert( queue );
    HeapPush(&queue->heap, key, data);
}
//****************************************************************************

void MinPriQueueInit(MinPriQueue* queue, int capacity)
{
    assert( queue && capacity > 0 );
    HeapInit(&(*queue).heap, capacity, MinPriQueueCompare, 0);
}
//****************************************************************************
HeapItem* GetPriQueueMin(MinPriQueue* queue)
{
    assert( queue );
    assert( queue->heap.size >= 1 );

    return &queue->heap.buffer[1];
}
//****************************************************************************
void MinPriQueuePop(MinPriQueue* queue, HeapItem* dst)
{
    assert( queue && dst );
    HeapPop(&queue->heap, dst);
}
//****************************************************************************
void MinPriQueuePush(MinPriQueue* queue, int key, void* data)
{
    assert( queue );
    HeapPush(&queue->heap, key, data);
}
//****************************************************************************