#include "process_manager.h"
#include "os_common.h"

// Global data managed by process manager.
List* gGlobalProcessList;
MinPriQueue* gTimerQueue;
MinPriQueue* gReadyQueue;

ProcessManager* gProcessManager;

//****************************************************************************
int GetTimerQueueProcessCount()
{
    if( gTimerQueue )
    {
        return gTimerQueue->heap.size;
    }

    return 0;
}
//****************************************************************************
PCB* GetTimerQueueProcess(int i)
{
    assert(i >= 0 && i < gTimerQueue->heap.size);
    return (PCB*)gTimerQueue->heap.buffer[i + 1].data;
}
//****************************************************************************
int GetReadyQueueProcessCount()
{
    if( gReadyQueue )
    {
        return gReadyQueue->heap.size;
    }

    return 0;
}
//****************************************************************************
int GetProcessCount()
{
    if( gGlobalProcessList )
    {
        return gGlobalProcessList->count;
    }

    return 0;
}
//****************************************************************************
PCB* GetPCBByID(long processID)
{
    ListNode* currentNode = gGlobalProcessList->head;
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
    ListNode* currentNode = gGlobalProcessList->head;
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
    ListNode* currentNode = gGlobalProcessList->head;
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
    ListNode* currentNode = gGlobalProcessList->head;
    if( ((PCB*)currentNode->data)->processID == processID )
    {
        free(gGlobalProcessList->head);
        gGlobalProcessList->head = 0;
        currentNode = 0;
        gGlobalProcessList->count--;

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
            gGlobalProcessList->count--;
            break;
        }
        currentNode = currentNode->next;
    }

    return;
}
//****************************************************************************
void RemoveFromTimerQueueByID(long processID)
{
    for( int i = 1; i <= gTimerQueue->heap.size; ++i )
    {
        HeapItem* item = &(gTimerQueue->heap.buffer[i]);
        if( ((PCB*)item->data)->processID == processID )
        {
            HeapItem temp;
            MinPriQueueRemove(gTimerQueue, i, &temp);
            break;
        }
    }
}
//****************************************************************************
void RemoveFromReadyQueueByID(long processID)
{
    for( int i = 1; i <= gReadyQueue->heap.size; ++i )
    {
        HeapItem* item = &(gReadyQueue->heap.buffer[i]);
        if( ((PCB*)item->data)->processID == processID )
        {
            HeapItem temp;
            MinPriQueueRemove(gReadyQueue, i, &temp);
            break;
        }
    }
}
//****************************************************************************
void PopFromTimerQueue(PCB** ppcb)
{
    HeapItem item;
    MinPriQueuePop(gTimerQueue, &item);
    *ppcb = (PCB*)item.data;
}
//****************************************************************************
void PushToTimerQueue(PCB* pcb)
{
    MinPriQueuePush(gTimerQueue, pcb->timerQueueKey, pcb);
}
//****************************************************************************
void PopFromReadyQueue(PCB** ppcb)
{
    HeapItem item;
    MinPriQueuePop(gReadyQueue, &item);
    *ppcb = (PCB*)item.data;
}
//****************************************************************************
void PushToReadyQueue(PCB* pcb)
{
    MinPriQueuePush(gReadyQueue, pcb->readyQueueKey, pcb);
}
//****************************************************************************
PCB* CreateProcess(char* name, int type, ProcessEntry entry, int priority, 
    long* dstID, long* dstErr)
{
    // Global process ID number is assigned to each process created.
    static long gCurrentProcessID = 0;

    // Create a PCB for the new process.
    PCB* pcb = ALLOC(PCB);
    pcb->type = type;
    pcb->entry = entry;
    pcb->priority = priority;
    pcb->currentPriority = priority;
    pcb->readyQueueKey = priority;
    size_t len = strlen(name);
    pcb->name = malloc(len + 1);
    strcpy(pcb->name, name);
    pcb->name[len] = 0;
    pcb->processID = ++gCurrentProcessID;

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
    ListNode* pcbNode = ALLOC(ListNode);
    pcbNode->data = (void*)pcb;
    ListAttach(gGlobalProcessList, pcbNode);

    // Add to ready queue.
    MinPriQueuePush(gReadyQueue, priority, pcb);

    // Create hardware context for the process.
    Z502MakeContext(&pcb->context, (void*)entry, USER_MODE);

    return pcb;
}
//****************************************************************************
void TerminateAllProcess()
{
    ListNode* currentProcessNode = gGlobalProcessList->head;
    for( int i = 0; i < gGlobalProcessList->count; ++i )
    {
        PCB* pcb = (PCB*)currentProcessNode->data;

        RemoveFromTimerQueueByID(pcb->processID);
        RemoveFromReadyQueueByID(pcb->processID);

        DEALLOC(pcb);

        currentProcessNode = currentProcessNode->next;
    }

    ListRelease(gGlobalProcessList);
}
//****************************************************************************
void TerminateProcess(long processID)
{
    RemoveFromTimerQueueByID(processID);
    RemoveFromReadyQueueByID(processID);
    RemovePCBFromRunningListByID(processID);
}
//****************************************************************************


//****************************************************************************
void ProcessManagerInitialize()
{
    // Create process manager.
    gProcessManager = ALLOC(ProcessManager);
    gProcessManager->CreateProcess = CreateProcess;
    gProcessManager->GetPCBByContext = GetPCBByContext;
    gProcessManager->GetPCBByID = GetPCBByID;
    gProcessManager->GetPCBByName = GetPCBByName;
    gProcessManager->GetProcessCount = GetProcessCount;
    gProcessManager->GetReadyQueueProcessCount = GetReadyQueueProcessCount;
    gProcessManager->GetTimerQueueProcessCount = GetTimerQueueProcessCount;
    gProcessManager->GetTimerQueueProcess = GetTimerQueueProcess;
    gProcessManager->PopFromReadyQueue = PopFromReadyQueue;
    gProcessManager->PopFromTimerQueue = PopFromTimerQueue;
    gProcessManager->PushToReadyQueue = PushToReadyQueue;
    gProcessManager->PushToTimerQueue = PushToTimerQueue;
    gProcessManager->RemoveFromReadyQueueByID = RemoveFromReadyQueueByID;
    gProcessManager->RemoveFromTimerQueueByID = RemoveFromTimerQueueByID;
    gProcessManager->RemovePCBFromRunningListByID = RemovePCBFromRunningListByID;
    gProcessManager->TerminateAllProcess = TerminateAllProcess;
    gProcessManager->TerminateProcess = TerminateProcess;

    // Init OS global variables.
    gGlobalProcessList = ALLOC(List);
    gTimerQueue = ALLOC(MinPriQueue);
    MinPriQueueInit(gTimerQueue, MAX_PROCESS_NUM);
    gReadyQueue = ALLOC(MinPriQueue);
    MinPriQueueInit(gReadyQueue, MAX_PROCESS_NUM);
}
//****************************************************************************
void ProcessManagerTerminate()
{
    DEALLOC(gProcessManager);
}
//****************************************************************************