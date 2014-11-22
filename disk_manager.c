//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#include "disk_manager.h"
#include "os_common.h"
#include "disk_sector_info.h"

List* gDiskOperationToDoList[MAX_NUMBER_OF_DISKS];
List* gDiskOperationWaitList[MAX_NUMBER_OF_DISKS];
DiskSectorInfo* gDiskStateTable[MAX_NUMBER_OF_DISKS];

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
    // The process will wait for disk operation.
    ListNode* pcbNode = (ListNode*)ALLOC(ListNode);
    pcbNode->data = (void*)diskOp;
    ListAttach(gDiskOperationWaitList[diskOp->diskID - 1], pcbNode);
    if( diskOp->operation == DISK_OP_READ ||
        diskOp->operation == DISK_OP_WRITE )
    {
        diskOp->requester->state = PROCESS_STATE_WAITING;
    }

    // Track disk usage. Set this sector of the disk is used.
    gDiskStateTable[diskOp->diskID - 1][diskOp->sector].user =
        diskOp->requester;
    if( diskOp->operation == DISK_OP_READ ||
        diskOp->operation == DISK_OP_WRITE )
    {
        gDiskStateTable[diskOp->diskID - 1][diskOp->sector].usage = DISK_STORE;
    }
    else
    {
        gDiskStateTable[diskOp->diskID - 1][diskOp->sector].usage = DISK_CACHE;
    }
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
void GetDiskCache(PCB* user, int* diskID, int* sector)
{
    *diskID = user->processID;
    int i;
    for( i = 0; i < NUM_LOGICAL_SECTORS; ++i )
    {
        // Find a free disk space.
        if( gDiskStateTable[*diskID - 1][i].user == 0 )
        {
            gDiskStateTable[*diskID - 1][i].user = user;
            gDiskStateTable[*diskID - 1][i].usage = DISK_CACHE;
            break;
        }
    }
    assert( i < NUM_LOGICAL_SECTORS );
    *sector = i;
}
//****************************************************************************
void FreeDiskCache(int diskID, int sector)
{
    gDiskStateTable[diskID - 1][sector].user = 0;
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
    gDiskManager->GetDiskCache = GetDiskCache;
    gDiskManager->FreeDiskCache = FreeDiskCache;

    // Init OS global variables.
    int i;
    for( i = 0; i < MAX_NUMBER_OF_DISKS; ++i )
    {
        gDiskOperationToDoList[i] = (List*)ALLOC(List);
        gDiskOperationWaitList[i] = (List*)ALLOC(List);
        gDiskStateTable[i] = calloc(NUM_LOGICAL_SECTORS, sizeof(DiskSectorInfo));
    }
}
//****************************************************************************
void DiskManagerTerminate()
{
    DEALLOC(gDiskManager);
}
//****************************************************************************