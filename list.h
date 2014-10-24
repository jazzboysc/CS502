//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#ifndef LIST_H_
#define LIST_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef struct ListNode
{
    void* data;
    struct ListNode* next;
} ListNode;

// A simple single list.
typedef struct _List
{
    ListNode* head;
    int count;
} List;

void ListRelease(List* list);
void ListAttach(List* list, ListNode* node);

#endif