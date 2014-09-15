#include "list.h"

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
}
//****************************************************************************