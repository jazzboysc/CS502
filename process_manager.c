#include "process_manager.h"
#include "os_common.h"

// Global data managed by process manager.
List*        gGlobalProcessList;
MinPriQueue* gTimerQueue;
MinPriQueue* gReadyQueue;
List*        gSuspendedList;
PCB*         gRunningProcess;

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
void RemovePCBFromGlobalListByID(long processID)
{
    if( ((PCB*)gGlobalProcessList->head->data)->processID == processID )
    {
        ListNode* temp = gGlobalProcessList->head;
        gGlobalProcessList->head = gGlobalProcessList->head->next;
        DEALLOC(temp);
        gGlobalProcessList->count--;

        return;
    }

    ListNode* prevNode = gGlobalProcessList->head;
    ListNode* currentNode = prevNode->next;
    while( currentNode )
    {
        if( ((PCB*)currentNode->data)->processID == processID )
        {
            prevNode->next = currentNode->next;
            DEALLOC(currentNode);
            gGlobalProcessList->count--;
            break;
        }

        prevNode = currentNode;
        currentNode = currentNode->next;
    }

    return;
}
//****************************************************************************
void RemoveFromTimerQueueByID(long processID)
{
    int i;
    for( i = 1; i <= gTimerQueue->heap.size; ++i )
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
    int i;
    for( i = 1; i <= gReadyQueue->heap.size; ++i )
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
    pcb->state = PROCESS_STATE_SLEEPING;
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
    pcb->state = PROCESS_STATE_READY;
}
//****************************************************************************
PCB* CreateProcess(char* name, int type, ProcessEntry entry, int priority, 
    long* dstID, long* dstErr)
{
    // Global process ID number is assigned to each process created.
    static long gCurrentProcessID = 0;

    // Create a PCB for the new process.
    PCB* pcb = (PCB*)ALLOC(PCB);
    pcb->type = type;
    pcb->entry = entry;
    pcb->priority = priority;
    pcb->currentPriority = priority;
    pcb->readyQueueKey = priority;
    size_t len = strlen(name);
    pcb->name = (char*)malloc(len + 1);
    strcpy(pcb->name, name);
    pcb->name[len] = 0;
    pcb->processID = ++gCurrentProcessID;
    pcb->messages = (List*)ALLOC(List);

    // Return process id to the caller.
    if( dstID )
    {
        *dstID = pcb->processID;
    }
    if( dstErr )
    {
        *dstErr = ERR_SUCCESS;
    }

    // Only add user process to global list and queue.
    if( type == PROCESS_TYPE_USER )
    {
        // Add to global process list.
        ListNode* pcbNode = (ListNode*)ALLOC(ListNode);
        pcbNode->data = (void*)pcb;
        ListAttach(gGlobalProcessList, pcbNode);

        // Add to ready queue.
        PushToReadyQueue(pcb);
    }

    // Create hardware context for the process.
    Z502MakeContext(&pcb->context, (void*)entry, USER_MODE);

    return pcb;
}
//****************************************************************************
void TerminateAllProcess()
{
    int i;
    ListNode* currentProcessNode = gGlobalProcessList->head;
    for( i = 0; i < gGlobalProcessList->count; ++i )
    {
        PCB* pcb = (PCB*)currentProcessNode->data;

        RemoveFromTimerQueueByID(pcb->processID);
        RemoveFromReadyQueueByID(pcb->processID);

        DEALLOC(pcb->messages);
        DEALLOC(pcb);

        currentProcessNode = currentProcessNode->next;
    }

    ListRelease(gGlobalProcessList);
}
//****************************************************************************
void TerminateProcess(PCB* pcb)
{
    pcb->state = PROCESS_STATE_DEAD;
    RemoveFromReadyQueueByID(pcb->processID);
}
//****************************************************************************
void SetRunningProcess(PCB* pcb)
{
    pcb->state = PROCESS_STATE_RUNNING;
    gRunningProcess = pcb;
}
//****************************************************************************
PCB* GetRunningProcess()
{
    return gRunningProcess;
}
//****************************************************************************
int IsAllDead()
{
    int isAllDead = 1;
    int i;

    ListNode* currentProcessNode = gGlobalProcessList->head;
    for( i = 0; i < gGlobalProcessList->count; ++i )
    {
        PCB* pcb = (PCB*)currentProcessNode->data;
        if( pcb->type == PROCESS_TYPE_USER && 
            pcb->state != PROCESS_STATE_DEAD &&
            pcb->state != PROCESS_STATE_SUSPENDED )
        {
            isAllDead = 0;
            break;
        }

        currentProcessNode = currentProcessNode->next;
    }

    return isAllDead;
}
//****************************************************************************
void AddToSuspendedList(PCB* pcb)
{
    ListNode* pcbNode = (ListNode*)ALLOC(ListNode);
    pcbNode->data = (void*)pcb;
    ListAttach(gSuspendedList, pcbNode);
    pcb->state = PROCESS_STATE_SUSPENDED;
}
//****************************************************************************
void RemoveFromSuspendedListByID(long processID)
{
    if( ((PCB*)gSuspendedList->head->data)->processID == processID )
    {
        ListNode* temp = gSuspendedList->head;
        gSuspendedList->head = gSuspendedList->head->next;
        DEALLOC(temp);
        gSuspendedList->count--;

        return;
    }

    ListNode* prevNode = gSuspendedList->head;
    ListNode* currentNode = prevNode->next;
    while( currentNode )
    {
        if( ((PCB*)currentNode->data)->processID == processID )
        {
            prevNode->next = currentNode->next;
            DEALLOC(currentNode);
            gSuspendedList->count--;
            break;
        }

        prevNode = currentNode;
        currentNode = currentNode->next;
    }

    return;
}
//****************************************************************************
void AddMessage(PCB* pcb, Message* msg)
{
    ListNode* node = (ListNode*)ALLOC(ListNode);
    node->data = (void*)msg;
    ListAttach(pcb->messages, node);

    if( pcb->state == PROCESS_STATE_SUSPENDED )
    {
        RemoveFromSuspendedListByID(pcb->processID);
        PushToReadyQueue(pcb);
    }
    else if( pcb->state == PROCESS_STATE_SUSPENDING )
    {
        pcb->state = PROCESS_STATE_SLEEPING;
    }
}
//****************************************************************************
Message* RemoveMessageBySenderID(PCB* pcb, long processID)
{
    Message* res = NULL;

    List* messages = pcb->messages;
    if( messages->count == 0 )
    {
        return res;
    }

    if( ((Message*)messages->head->data)->senderProcessID == processID )
    {
        res = (Message*)messages->head->data;
        ListNode* temp = messages->head;
        messages->head = messages->head->next;
        DEALLOC(temp);
        messages->count--;
        return res;
    }

    ListNode* prev = messages->head;
    ListNode* current = prev->next;
    while( current )
    {
        if( ((Message*)current->data)->senderProcessID == processID )
        {
            prev->next = current->next;
            res = (Message*)current->data;
            DEALLOC(current);
            messages->count--;
            break;
        }

        prev = current;
        current = current->next;
    }

    return res;
}
//****************************************************************************
Message* GetMessageBySenderID(PCB* pcb, long processID)
{
    ListNode* currentMessageNode = pcb->messages->head;
    while( currentMessageNode )
    {
        if( ((Message*)currentMessageNode->data)->senderProcessID == processID )
        {
            return (Message*)currentMessageNode->data;
        }
    }

    return NULL;
}
//****************************************************************************
int GetMessageListCount(PCB* pcb)
{
    if( !pcb->messages )
    {
        return 0;
    }

    return pcb->messages->count;
}
//****************************************************************************
int BroadcastMessage(long senderProcessID, Message* msg)
{
    int res = 0;
    int i;

    ListNode* currentProcessNode = gGlobalProcessList->head;
    for( i = 0; i < gGlobalProcessList->count; ++i )
    {
        PCB* receiver = (PCB*)currentProcessNode->data;
        if( receiver->processID != senderProcessID &&
            receiver->type == PROCESS_TYPE_USER &&
            receiver->state != PROCESS_STATE_DEAD )
        {
            int messageCount = GetMessageListCount(receiver);
            if( messageCount < MAX_MESSAGE_LIST_NUM )
            {
                Message* temp = (Message*)ALLOC(Message);
                memcpy(temp, msg, sizeof(Message));
                AddMessage(receiver, temp);
            }
            else
            {
                res = 1;
            }
        }

        currentProcessNode = currentProcessNode->next;
    }

    return res;
}
//****************************************************************************
Message* GetFirstMessage(PCB* pcb)
{
    if( pcb->messages->count > 0 )
    {
        return (Message*)pcb->messages->head->data;
    }

    return NULL;
}
//****************************************************************************
void PrintState()
{
    INT32 currentTime;
    int i;
    int sleepCount;

    MEM_READ(Z502ClockStatus, &currentTime);
    SP_setup(SP_TIME_MODE, currentTime);
    SP_setup(SP_RUNNING_MODE, GetRunningProcess()->processID);

    MinPriQueue tempQueue;
    HeapItem tempItem;
    MinPriQueueClone(gReadyQueue, &tempQueue);
    int readyCount = tempQueue.heap.size;
    for( i = 0; i < readyCount; ++i )
    {
        MinPriQueuePop(&tempQueue, &tempItem);
        SP_setup(SP_READY_MODE, ((PCB*)tempItem.data)->processID);
    }

    MinPriQueueClone(gTimerQueue, &tempQueue);
    sleepCount = tempQueue.heap.size;
    for( i = 0; i < sleepCount; ++i )
    {
        MinPriQueuePop(&tempQueue, &tempItem);
        SP_setup(SP_TIMER_SUSPENDED_MODE, ((PCB*)tempItem.data)->processID);
    }

    ListNode* current = gSuspendedList->head;
    while( current )
    {
        SP_setup(SP_PROCESS_SUSPENDED_MODE, ((PCB*)current->data)->processID);
        current = current->next;
    }

    SP_print_line();
}
//****************************************************************************

//****************************************************************************
void ProcessManagerInitialize()
{
    // Create process manager.
    gProcessManager = (ProcessManager*)ALLOC(ProcessManager);
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
    gProcessManager->RemovePCBFromGlobalListByID = RemovePCBFromGlobalListByID;
    gProcessManager->TerminateAllProcess = TerminateAllProcess;
    gProcessManager->TerminateProcess = TerminateProcess;
    gProcessManager->SetRunningProcess = SetRunningProcess;
    gProcessManager->GetRunningProcess = GetRunningProcess;
    gProcessManager->IsAllDead = IsAllDead;
    gProcessManager->AddToSuspendedList = AddToSuspendedList;
    gProcessManager->RemoveFromSuspendedListByID = RemoveFromSuspendedListByID;
    gProcessManager->AddMessage = AddMessage;
    gProcessManager->RemoveMessageBySenderID = RemoveMessageBySenderID;
    gProcessManager->GetMessageBySenderID = GetMessageBySenderID;
    gProcessManager->GetMessageListCount = GetMessageListCount;
    gProcessManager->BroadcastMessage = BroadcastMessage;
    gProcessManager->GetFirstMessage = GetFirstMessage;
    gProcessManager->PrintState = PrintState;

    // Init OS global variables.
    gGlobalProcessList = (List*)ALLOC(List);
    gTimerQueue = (MinPriQueue*)ALLOC(MinPriQueue);
    MinPriQueueInit(gTimerQueue, MAX_PROCESS_NUM);
    gReadyQueue = (MinPriQueue*)ALLOC(MinPriQueue);
    MinPriQueueInit(gReadyQueue, MAX_PROCESS_NUM);
    gSuspendedList = (List*)ALLOC(List);
    gRunningProcess = NULL;
}
//****************************************************************************
void ProcessManagerTerminate()
{
    DEALLOC(gProcessManager);
}
//****************************************************************************