#include "process_manager.h"
#include "os_common.h"

List* RunningList;
MinPriQueue* TimerQueue;
MinPriQueue* ReadyQueue;

//****************************************************************************
void ProcessManagerInit()
{
    // Init OS global variables.
    RunningList = calloc(1, sizeof(List));
    TimerQueue = calloc(1, sizeof(MinPriQueue));
    MinPriQueueInit(TimerQueue, MAX_PROCESS_NUM);
    ReadyQueue = calloc(1, sizeof(MinPriQueue));
    MinPriQueueInit(ReadyQueue, MAX_PROCESS_NUM);
}
//****************************************************************************
int GetProcessCount()
{
    if( RunningList )
    {
        return RunningList->count;
    }

    return 0;
}
//****************************************************************************
PCB* GetPCBByID(long processID)
{
    ListNode* currentNode = RunningList->head;
    while( currentNode )
    {
        if( ((PCB*)currentNode->data)->processID == processID )
        {
            return (PCB*)currentNode->data;
        }
        currentNode = currentNode->next;
    }

    return 0;
}
//****************************************************************************
PCB* GetPCBByName(char* name)
{
    ListNode* currentNode = RunningList->head;
    while( currentNode )
    {
        if( strcmp(((PCB*)currentNode->data)->name, name) == 0 )
        {
            return (PCB*)currentNode->data;
        }
        currentNode = currentNode->next;
    }

    return 0;
}
//****************************************************************************
PCB* GetPCBByContext(void* context)
{
    ListNode* currentNode = RunningList->head;
    while( currentNode )
    {
        if( ((PCB*)currentNode->data)->context == context )
        {
            return (PCB*)currentNode->data;
        }
        currentNode = currentNode->next;
    }

    return 0;
}
//****************************************************************************
void RemovePCBFromRunningListByID(long processID)
{
    ListNode* currentNode = RunningList->head;
    if( ((PCB*)currentNode->data)->processID == processID )
    {
        free(RunningList->head);
        RunningList->head = 0;
        currentNode = 0;
        RunningList->count--;

        return;
    }

    ListNode* prevNode = currentNode;
    currentNode = currentNode->next;
    while( currentNode )
    {
        if( ((PCB*)currentNode->data)->processID == processID )
        {
            prevNode->next = currentNode->next;
            free(currentNode);
            RunningList->count--;
            break;
        }
        currentNode = currentNode->next;
    }

    return;
}
//****************************************************************************
void RemoveFromTimerQueueByID(long processID)
{
    for( int i = 1; i <= TimerQueue->heap.size; ++i )
    {
        HeapItem* item = &(TimerQueue->heap.buffer[i]);
        if( ((PCB*)item->data)->processID == processID )
        {
            HeapItem temp;
            MinPriQueueRemove(TimerQueue, i, &temp);
            break;
        }
    }
}
//****************************************************************************
void RemoveFromReadyQueueByID(long processID)
{
    for( int i = 1; i <= ReadyQueue->heap.size; ++i )
    {
        HeapItem* item = &(ReadyQueue->heap.buffer[i]);
        if( ((PCB*)item->data)->processID == processID )
        {
            HeapItem temp;
            MinPriQueueRemove(ReadyQueue, i, &temp);
            break;
        }
    }
}
//****************************************************************************
void PopFromTimerQueue(PCB** ppcb)
{
    HeapItem item;
    MinPriQueuePop(TimerQueue, &item);
    *ppcb = (PCB*)item.data;
}
//****************************************************************************
void PushToTimerQueue(PCB* pcb)
{
    MinPriQueuePush(TimerQueue, pcb->timerQueueKey, pcb);
}
//****************************************************************************
void PopFromReadyQueue(PCB** ppcb)
{
    HeapItem item;
    MinPriQueuePop(ReadyQueue, &item);
    *ppcb = (PCB*)item.data;
}
//****************************************************************************
void PushToReadyQueue(PCB* pcb)
{
    MinPriQueuePush(ReadyQueue, pcb->readyQueueKey, pcb);
}
//****************************************************************************
PCB* OSCreateProcess(char* name, ProcessEntry entry, int priority, long* dstID,
    long* dstErr)
{
    static long CurrentProcessID = 0;

    PCB* pcb = calloc(1, sizeof(PCB));
    pcb->entry = entry;
    pcb->priority = priority;
    pcb->readyQueueKey = priority;
    size_t len = strlen(name);
    pcb->name = malloc(len + 1);
    strcpy(pcb->name, name);
    pcb->name[len] = 0;
    pcb->processID = ++CurrentProcessID;

    // Return process id to the caller.
    if( dstID )
    {
        *dstID = pcb->processID;
    }
    if( dstErr )
    {
        *dstErr = ERR_SUCCESS;
    }

    // Add to global process list.
    ListNode* pcbNode = calloc(1, sizeof(ListNode));
    pcbNode->data = (void*)pcb;
    ListAttach(RunningList, pcbNode);

    // Add to ready queue.
    MinPriQueuePush(ReadyQueue, priority, pcb);

    Z502MakeContext(&pcb->context, (void*)entry, USER_MODE);

    return pcb;
}
//****************************************************************************