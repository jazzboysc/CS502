#include "priority_queue.h"
#include <assert.h>

//****************************************************************************
MaxHeapItem* GetPriQueueMax(MaxPriQueue* queue)
{
    assert( queue );
    assert( queue->heap->size >= 1 );

    return &queue->heap->buffer[1];
}
//****************************************************************************
void PopPriQueueMax(MaxPriQueue* queue, MaxHeapItem* item)
{
    assert( queue && item );
    assert( queue->heap->size >= 1 );

    *item = queue->heap->buffer[1];
    queue->heap->buffer[1] = queue->heap->buffer[queue->heap->size];
    queue->heap->size--;
    if( queue->heap->size > 0 )
    {
        MaxHeapify(queue->heap, 1);
    }
}
//****************************************************************************