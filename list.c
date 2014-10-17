//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#include "list.h"

//****************************************************************************
void ListRelease(List* list)
{
    if( list )
    {
        ListNode* current = list->head;
        ListNode* next = current->next;
        free(current);
        list->head = 0;

        while( next )
        {
            current = next;
            next = current->next;
            free(current);
        }
    }
}
//****************************************************************************
void ListAttach(List* list, ListNode* node)
{
    if( !list->head )
    {
        list->head = node;
    }
    else
    {
        ListNode* current = list->head;
        while( current->next )
        {
            current = current->next;
        }
        current->next = node;
    }

    list->count++;
}
//****************************************************************************