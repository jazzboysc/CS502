#ifndef HEAP_H_
#define HEAP_H_

typedef struct _MaxHeapItem
{
    int key;
    void* data;
} MaxHeapItem;

typedef struct _MaxHeap
{
    MaxHeapItem* buffer;
    int size;
    int capacity;
} MaxHeap;

//****************************************************************************
#define HEAP_GET_PARENT_INDEX(i) i >> 1
//****************************************************************************
#define HEAP_GET_LEFT_CHILD_INDEX(i) i << 1
//****************************************************************************
#define HEAP_GET_RIGHT_CHILD_INDEX(i) (i << 1) + 1
//****************************************************************************
#define HEAP_GET_KEY(heap, i) heap->buffer[i].key
//****************************************************************************
#define HEAP_SET_KEY(heap, i, newKey) heap->buffer[i].key = newKey
//****************************************************************************
#define HEAP_GET_DATA(heap, i) heap->buffer[i].data
//****************************************************************************
#define HEAP_SET_DATA(heap, i, newData) heap->buffer[i].data = newData
//****************************************************************************
#define HEAP_SWAP(heap, i, j) \
    MaxHeapItem temp; \
    temp = heap->buffer[i]; \
    heap->buffer[i] = heap->buffer[j]; \
    heap->buffer[j] = temp; \
//****************************************************************************

void MaxHeapify(MaxHeap* heap, int i);
void BuildMaxHeap(MaxHeap* heap);
void MaxHeapUpdateKey(MaxHeap* heap, int i, int newKey);
void MaxHeapInsert(MaxHeap* heap, int key, void* data);

#endif