#include "svc.h"
#include "pcb.h"
#include "os_common.h"
#include "process_manager.h"

extern List* RunningList;
extern MinPriQueue* TimerQueue;
extern MinPriQueue* ReadyQueue;

//****************************************************************************
void SVCGetProcessID(SYSTEM_CALL_DATA* SystemCallData)
{
    size_t nameLen = strlen((char*)SystemCallData->Argument[0]);
    if( nameLen == 0 )
    {
        // Find caller's PCB.
        void* currentContext = (void*)Z502_CURRENT_CONTEXT;
        PCB* pcb = GetPCBByContext(currentContext);

        long processID = pcb->processID;
        *(long*)SystemCallData->Argument[1] = processID;
        *(long*)SystemCallData->Argument[2] = ERR_SUCCESS;
    }
    else
    {
        PCB* pcb = GetPCBByName((char*)SystemCallData->Argument[0]);
        if( pcb )
        {
            long processID = pcb->processID;
            *(long*)SystemCallData->Argument[1] = processID;
            *(long*)SystemCallData->Argument[2] = ERR_SUCCESS;
        }
        else
        {
            *(long*)SystemCallData->Argument[2] = ERR_PROCESS_ID_NOT_FOUND;
        }
    }
}
//****************************************************************************
void SVCTerminateProcess(SYSTEM_CALL_DATA* SystemCallData)
{
    if( (INT32)SystemCallData->Argument[0] == -2 )
    {
        // Terminate all.

        ListNode* currentProcessNode = RunningList->head;
        for( int i = 0; i < RunningList->count; ++i )
        {
            PCB* pcb = (PCB*)currentProcessNode->data;

            RemoveFromTimerQueueByID(pcb->processID);
            RemoveFromReadyQueueByID(pcb->processID);

            free(pcb);

            currentProcessNode = currentProcessNode->next;
        }

        ListRelease(RunningList);
        Z502Halt();
    }
    else if( (INT32)SystemCallData->Argument[0] == -1 )
    {
        // Terminate myself.

        // Find caller's PCB.
        void* currentContext = (void*)Z502_CURRENT_CONTEXT;
        PCB* pcb = GetPCBByContext(currentContext);
        long processID = pcb->processID;

        // Remove from global list and queues.
        RemoveFromTimerQueueByID(processID);
        RemoveFromReadyQueueByID(processID);
        RemovePCBFromRunningListByID(processID);

        free(pcb);

        if( processID == 1 )
        {
            Z502Halt();
        }
    }
    else
    {
        // Terminate by id.

        long processID = (long)SystemCallData->Argument[0];
        PCB* pcb = GetPCBByID(processID);

        if( !pcb )
        {
            *(long*)SystemCallData->Argument[1] =
                ERR_PROCESS_ID_NOT_FOUND;
            return;
        }

        // Remove from global list and queues.
        RemoveFromTimerQueueByID(processID);
        RemoveFromReadyQueueByID(processID);
        RemovePCBFromRunningListByID(processID);

        //Z502DestroyContext(&pcb->context);
        free(pcb);
    }

    if( SystemCallData->Argument[1] )
    {
        *(long*)SystemCallData->Argument[1] = ERR_SUCCESS;
    }
}
//****************************************************************************
void SVCCreateProcess(SYSTEM_CALL_DATA* SystemCallData)
{
    long* dstErr = (long*)SystemCallData->Argument[4];

    int processCount = GetProcessCount();
    if( processCount >= MAX_PROCESS_NUM )
    {
        if( dstErr )
        {
            *dstErr = ERR_CREAT_PROCESS_REACH_MAX_NUM;
        }
        return;
    }

    if( (INT32)SystemCallData->Argument[2] == ILLEGAL_PRIORITY )
    {
        if( dstErr )
        {
            *dstErr = ERR_CREAT_PROCESS_ILLEGAL_PRIORITY;
        }
        return;
    }

    char* name = (char*)SystemCallData->Argument[0];
    if( !name )
    {
        if( dstErr )
        {
            *dstErr = ERR_CREAT_PROCESS_ILLEGAL_NAME;
        }
        return;
    }

    PCB* oldPcb = GetPCBByName(name);
    if( oldPcb )
    {
        if( dstErr )
        {
            *dstErr = ERR_CREAT_PROCESS_ILLEGAL_NAME;
        }
        return;
    }

    ProcessEntry entry = (ProcessEntry)SystemCallData->Argument[1];
    if( !entry )
    {
        if( dstErr )
        {
            *dstErr = ERR_CREAT_PROCESS_ILLEGAL_ENTRY;
        }
        return;
    }

    OSCreateProcess((char*)SystemCallData->Argument[0],
        (ProcessEntry)SystemCallData->Argument[1],
        (int)SystemCallData->Argument[2],
        (long*)SystemCallData->Argument[3],
        (long*)SystemCallData->Argument[4]);
}
//****************************************************************************
void SVCStartTimer(SYSTEM_CALL_DATA* SystemCallData)
{
    INT32 currentTime;
    INT32 Status;
    INT32 sleepTime;
    void* currentContext;

    // Find caller's PCB.
    currentContext = (void*)Z502_CURRENT_CONTEXT;
    PCB* pcb = GetPCBByContext(currentContext);

    CALL(MEM_READ(Z502ClockStatus, &currentTime));
    sleepTime = (INT32)SystemCallData->Argument[0];
    pcb->timerQueueKey = currentTime + sleepTime;

    RemoveFromReadyQueueByID(pcb->processID);
    PushToTimerQueue(pcb);

    CALL(MEM_READ(Z502TimerStatus, &Status));
    if( Status == DEVICE_FREE )
    {
        printf("Timer is free\n");
    }
    else
    {
        printf("Timer is busy\n");
    }

    CALL(MEM_WRITE(Z502TimerStart, &sleepTime));
    CALL(MEM_READ(Z502TimerStatus, &Status));
    if( Status == DEVICE_IN_USE )
    {
        printf("Timer started\n");
    }
    else
    {
        printf("Unable to start timer\n");
    }

    Z502Idle();
}
//****************************************************************************