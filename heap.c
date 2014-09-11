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