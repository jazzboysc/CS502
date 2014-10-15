#include "heap.h"
#include <limits.h>

//****************************************************************************
void HeapInit(Heap* heap, int capacity, HeapCompareKey compare, int isMaxHeap)
{
    assert( heap && compare && capacity > 0 );

    heap->buffer = (HeapItem*)malloc(sizeof(HeapItem)*(capacity + 1));
    memset(heap->buffer, 0x00, sizeof(HeapItem)*(capacity + 1));
    heap->size = 0;
    heap->capacity = capacity;
    heap->compare = compare;
    heap->timeStamp = 0;
    heap->isMaxHeap = isMaxHeap;
}
//****************************************************************************
void HeapAdjust(Heap* heap, int i)
{
    assert( heap );
    assert( i >= 1 && i <= heap->size );
    assert( heap->compare );

    int target;
    int left = HEAP_GET_LEFT_CHILD_INDEX(i);
    int right = HEAP_GET_RIGHT_CHILD_INDEX(i);

    if( left <= heap->size && 
        heap->compare(&HEAP_GET_KEY(heap, left), &HEAP_GET_KEY(heap, i)) > 0 )
    {
        target = left;
    }
    else
    {
        target = i;
    }
    if( right <= heap->size && 
        heap->compare(&HEAP_GET_KEY(heap, right), &HEAP_GET_KEY(heap, target)) > 0 )
    {
        target = right;
    }

    if( target != i )
    {
        HEAP_SWAP(heap, i, target);
        HeapAdjust(heap, target);
    }
}
//****************************************************************************
void HeapBuild(Heap* heap)
{
    assert( heap );

    int i;
    for( i = heap->size / 2; i >= 1; i-- )
    {
        HeapAdjust(heap, i);
    }
}
//****************************************************************************
void HeapUpdateKey(Heap* heap, int i, int newKey)
{
    assert( heap );
    assert( i >= 1 && i <= heap->size );
    assert( heap->compare );

    HeapItemKey oldKey = HEAP_GET_KEY(heap, i);
    HeapItemKey tempKey;
    tempKey.key = newKey;
    tempKey.timeStamp = oldKey.timeStamp;
    HEAP_SET_KEY(heap, i, tempKey);

    int res = heap->compare(&tempKey, &oldKey);
    if( res > 0 )
    {
        while( i > 1 )
        {
            int parent = HEAP_GET_PARENT_INDEX(i);
            HeapItemKey parentKey = HEAP_GET_KEY(heap, parent);
            if( heap->compare(&tempKey, &parentKey) > 0 )
            {
                HEAP_SWAP(heap, i, parent);
                i = parent;
            }
            else
            {
                break;
            }
        }
    }
    else if( res < 0 )
    {
        HeapAdjust(heap, i);
    }
}
//****************************************************************************
void HeapPush(Heap* heap, int key, void* data)
{
    assert( heap );

    if( heap->size == heap->capacity )
    {
        assert( 0 );
    }

    heap->size++;
    heap->timeStamp++;

    HeapItemKey tempKey;
    tempKey.key = heap->isMaxHeap ? INT_MIN : INT_MAX;
    tempKey.timeStamp = heap->timeStamp;

    HEAP_SET_KEY(heap, heap->size, tempKey);
    HEAP_SET_DATA(heap, heap->size, data);
    HeapUpdateKey(heap, heap->size, key);
}
//****************************************************************************
void HeapPop(Heap* heap, HeapItem* dst)
{
    assert( heap && dst );
    assert( heap->size >= 1 );

    *dst = heap->buffer[1];
    heap->buffer[1] = heap->buffer[heap->size];
    heap->size--;
    if( heap->size > 0 )
    {
        HeapAdjust(heap, 1);
    }
}
//****************************************************************************
void HeapRemove(Heap* heap, int i, HeapItem* dst)
{
    assert( heap && dst );
    assert( i >= 1 && i <= heap->size );

    *dst = heap->buffer[i];
    if( i == heap->size )
    {
        heap->size--;
    }
    else
    {
        heap->buffer[i] = heap->buffer[heap->size];
        heap->size--;
        HeapAdjust(heap, i);
    }
}
//****************************************************************************
void HeapClone(Heap* src, Heap* dst)
{
    assert( src && dst );

    memcpy(dst, src, sizeof(Heap));
    dst->buffer = (HeapItem*)malloc(sizeof(HeapItem)*(dst->capacity + 1));
    memcpy(dst->buffer, src->buffer, sizeof(HeapItem)*(dst->capacity + 1));
}
//****************************************************************************