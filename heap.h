//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#ifndef HEAP_H_
#define HEAP_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef struct _HeapItemKey
{
    int key;
    int timeStamp;
} HeapItemKey;

typedef struct _HeapItem
{
    HeapItemKey key;
    void* data;
} HeapItem;

typedef int(*HeapCompareKey)(HeapItemKey* key1, HeapItemKey* key2);

// A generic heap.
typedef struct _Heap
{
    HeapItem* buffer;
    int size;
    int capacity;
    HeapCompareKey compare;
    unsigned int timeStamp;
    int isMaxHeap;
} Heap;

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
    HeapItem temp; \
    temp = heap->buffer[i]; \
    heap->buffer[i] = heap->buffer[j]; \
    heap->buffer[j] = temp; \
//****************************************************************************

void HeapInit(Heap* heap, int capacity, HeapCompareKey compare, int isMaxHeap);
void HeapAdjust(Heap* heap, int i);
void HeapBuild(Heap* heap);
void HeapUpdateKey(Heap* heap, int i, int newKey);
void HeapPush(Heap* heap, int key, void* data);
void HeapPop(Heap* heap, HeapItem* dst);
void HeapRemove(Heap* heap, int i, HeapItem* dst);
void HeapClone(Heap* src, Heap* dst);

#endif