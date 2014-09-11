#include "heap.h"
#include <assert.h>

//****************************************************************************
void MaxHeapify(MaxHeap* heap, int i)
{
    assert( heap );
    assert( i >= 1 && i <= heap->size );

    int largest;
    int left = HEAP_GET_LEFT_CHILD_INDEX(i);
    int right = HEAP_GET_RIGHT_CHILD_INDEX(i);

    if( left <= heap->size && 
        HEAP_GET_KEY(heap, left) > HEAP_GET_KEY(heap, i) )
    {
        largest = left;
    }
    else
    {
        largest = i;
    }
    if( right <= heap->size && 
        HEAP_GET_KEY(heap, right) > HEAP_GET_KEY(heap, largest) )
    {
        largest = right;
    }

    if( largest != i )
    {
        HEAP_SWAP(heap, i, largest);
        MaxHeapify(heap, largest);
    }
}
//****************************************************************************
void BuildMaxHeap(MaxHeap* heap)
{
    assert( heap );

    int i;
    for( i = heap->size / 2; i >= 1; i-- )
    {
        MaxHeapify(heap, i);
    }
}
//****************************************************************************
void MaxHeapUpdateKey(MaxHeap* heap, int i, int newKey)
{
    assert( heap );
    assert( i >= 1 && i <= heap->size );

    int oldKey = HEAP_GET_KEY(heap, i);
    HEAP_SET_KEY(heap, i, newKey);

    if( newKey > oldKey )
    {
        while( i > 1 )
        {
            int parent = HEAP_GET_PARENT_INDEX(i);
            int parentKey = HEAP_GET_KEY(heap, parent);
            if( newKey > parentKey )
            {
                HEAP_SWAP(heap, i, parent);
                i = parent;
            }
        }
    }
    else if( newKey < oldKey )
    {
        MaxHeapify(heap, i);
    }
}
//****************************************************************************
void MaxHeapInsert(MaxHeap* heap, int key, void* data)
{
    assert( heap );

    if( heap->size == heap->capacity )
    {
    }

    heap->size++;
    HEAP_SET_KEY(heap, heap->size, -1);
    HEAP_SET_DATA(heap, heap->size, data);
    MaxHeapUpdateKey(heap, heap->size, key);
}
//****************************************************************************