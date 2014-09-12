#include "heap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

//****************************************************************************
void HeapInit(Heap* heap, int capacity, HeapCompareKey compare)
{
    assert( heap && compare && capacity > 0 );

    heap->buffer = malloc(sizeof(HeapItem)*capacity);
    memset(heap->buffer, 0x00, sizeof(HeapItem)*capacity);
    heap->size = 0;
    heap->capacity = capacity;
    heap->compare = compare;
    heap->timeStamp = 0;
}
//****************************************************************************
void HeapAdjust(Heap* heap, int i)
{
    assert( heap );
    assert( i >= 1 && i <= heap->size );
    assert( heap->compare );

    int largest;
    int left = HEAP_GET_LEFT_CHILD_INDEX(i);
    int right = HEAP_GET_RIGHT_CHILD_INDEX(i);

    if( left <= heap->size && 
        heap->compare(&HEAP_GET_KEY(heap, left), &HEAP_GET_KEY(heap, i)) > 0 )
    {
        largest = left;
    }
    else
    {
        largest = i;
    }
    if( right <= heap->size && 
        heap->compare(&HEAP_GET_KEY(heap, right), &HEAP_GET_KEY(heap, largest)) > 0 )
    {
        largest = right;
    }

    if( largest != i )
    {
        HEAP_SWAP(heap, i, largest);
        HeapAdjust(heap, largest);
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
void HeapUpdateKey(Heap* heap, int i, HeapItemKey* newKey)
{
    assert( heap );
    assert( i >= 1 && i <= heap->size );
    assert( heap->compare );

    HeapItemKey oldKey = HEAP_GET_KEY(heap, i);
    HEAP_SET_KEY(heap, i, *newKey);

    int res = heap->compare(newKey, &oldKey);
    if( res > 0 )
    {
        while( i > 1 )
        {
            int parent = HEAP_GET_PARENT_INDEX(i);
            HeapItemKey parentKey = HEAP_GET_KEY(heap, parent);
            if( heap->compare(newKey, &parentKey) > 0 )
            {
                HEAP_SWAP(heap, i, parent);
                i = parent;
            }
        }
    }
    else if( res < 0 )
    {
        HeapAdjust(heap, i);
    }
}
//****************************************************************************
void HeapInsert(Heap* heap, HeapItemKey* key, void* data)
{
    assert( heap );

    if( heap->size == heap->capacity )
    {
        assert( 0 );
    }

    heap->size++;
    heap->timeStamp++;

    HeapItemKey tempMin;
    tempMin.key = -1;
    tempMin.timeStamp = 0;

    HEAP_SET_KEY(heap, heap->size, tempMin);
    HEAP_SET_DATA(heap, heap->size, data);
    HeapUpdateKey(heap, heap->size, key);
}
//****************************************************************************