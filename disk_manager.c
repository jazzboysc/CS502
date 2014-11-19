//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#include "disk_manager.h"
#include "os_common.h"

List* gDiskOperationToDoList[MAX_NUMBER_OF_DISKS];
List* gDiskOperationWaitList[MAX_NUMBER_OF_DISKS];

DiskManager* gDiskManager;

//****************************************************************************
void PushToDiskOperationToDoList(DiskOperation* diskOp)
{
    ListNode* pcbNode = (ListNode*)ALLOC(ListNode);
    pcbNode->data = (void*)diskOp;
    ListAttach(gDiskOperationToDoList[diskOp->diskID - 1], pcbNode);
}
//****************************************************************************
void PopFromDiskOperationToDoList(int diskID, DiskOperation** diskOp)
{
    ListNode* head = gDiskOperationToDoList[diskID - 1]->head;
    if( head == NULL )
    {
        // Nothing to pop.
        *diskOp = NULL;
        return;
    }

    gDiskOperationToDoList[diskID - 1]->head =
        gDiskOperationToDoList[diskID - 1]->head->next;
    gDiskOperationToDoList[diskID - 1]->count--;

    *diskOp = (DiskOperation*)head->data;
    DEALLOC(head);
}
//****************************************************************************
void PushToDiskOperationWaitList(DiskOperation* diskOp)
{
    ListNode* pcbNode = (ListNode*)ALLOC(ListNode);
    pcbNode->data = (void*)diskOp;
    ListAttach(gDiskOperationWaitList[diskOp->diskID - 1], pcbNode);
    diskOp->requester->state = PROCESS_STATE_WAITING;
}
//****************************************************************************
void PopFromDiskOperationWaitList(int diskID, DiskOperation** diskOp)
{
    ListNode* head = gDiskOperationWaitList[diskID - 1]->head;
    if( head == NULL )
    {
        // Nothing to pop.
        *diskOp = NULL;
        return;
    }

    gDiskOperationWaitList[diskID - 1]->head =
        gDiskOperationWaitList[diskID - 1]->head->next;
    gDiskOperationWaitList[diskID - 1]->count--;

    *diskOp = (DiskOperation*)head->data;
    DEALLOC(head);
}
//****************************************************************************

//****************************************************************************
void DiskManagerInitialize()
{
    // Create disk manager.
    gDiskManager = (DiskManager*)ALLOC(DiskManager);
    gDiskManager->PushToDiskOperationToDoList = PushToDiskOperationToDoList;
    gDiskManager->PopFromDiskOperationToDoList = PopFromDiskOperationToDoList;
    gDiskManager->PushToDiskOperationWaitList = PushToDiskOperationWaitList;
    gDiskManager->PopFromDiskOperationWaitList = PopFromDiskOperationWaitList;

    // Init OS global variables.
    int i;
    for( i = 0; i < MAX_NUMBER_OF_DISKS; ++i )
    {
        gDiskOperationToDoList[i] = (List*)ALLOC(List);
        gDiskOperationWaitList[i] = (List*)ALLOC(List);
    }
}
//****************************************************************************
void DiskManagerTerminate()
{
    DEALLOC(gDiskManager);
}
//****************************************************************************