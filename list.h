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

typedef struct _List
{
    ListNode* head;
} List;

void ListAttach(List* list, ListNode* node);

#endif