#include "svc.h"
#include "pcb.h"
#include "os_common.h"
#include "process_manager.h"
#include "scheduler.h"
#include "critical_section.h"

//****************************************************************************
void SVCGetProcessID(SYSTEM_CALL_DATA* SystemCallData)
{
    size_t nameLen = strlen((char*)SystemCallData->Argument[0]);
    if( nameLen == 0 )
    {
        // Find caller's PCB.
        PCB* pcb = gProcessManager->GetRunningProcess();
        if( pcb->state == PROCESS_STATE_DEAD )
        {
            *(long*)SystemCallData->Argument[2] = ERR_PROCESS_ID_NOT_FOUND;
        }
        else
        {
            long processID = pcb->processID;
            *(long*)SystemCallData->Argument[1] = processID;
            *(long*)SystemCallData->Argument[2] = ERR_SUCCESS;
        }
    }
    else
    {
        PCB* pcb = gProcessManager->GetPCBByName((char*)SystemCallData->Argument[0]);
        if( pcb )
        {
            if( pcb->state == PROCESS_STATE_DEAD )
            {
                *(long*)SystemCallData->Argument[2] = ERR_PROCESS_ID_NOT_FOUND;
            }
            else
            {
                long processID = pcb->processID;
                *(long*)SystemCallData->Argument[1] = processID;
                *(long*)SystemCallData->Argument[2] = ERR_SUCCESS;
            }
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
    EnterCriticalSection(0);

    if( (INT32)SystemCallData->Argument[0] == -2 )
    {
        // Terminate all.
        Z502Halt();
    }
    else if( (INT32)SystemCallData->Argument[0] == -1 )
    {
        // Terminate myself.

        // Find caller's PCB.
        PCB* pcb = gProcessManager->GetRunningProcess();
        long processID = pcb->processID;

        gProcessManager->TerminateProcess(pcb);

        if( processID == 1 )
        {
            Z502Halt();
        }
    }
    else
    {
        // Terminate by id.

        long processID = (long)SystemCallData->Argument[0];
        PCB* pcb = gProcessManager->GetPCBByID(processID);

        if( !pcb )
        {
            *(long*)SystemCallData->Argument[1] =
                ERR_PROCESS_ID_NOT_FOUND;
            return;
        }

        gProcessManager->TerminateProcess(pcb);
    }

    if( SystemCallData->Argument[1] )
    {
        *(long*)SystemCallData->Argument[1] = ERR_SUCCESS;
    }

    LeaveCriticalSection(0);

    gScheduler->OnProcessTerminate();
}
//****************************************************************************
void SVCCreateProcess(SYSTEM_CALL_DATA* SystemCallData)
{
    long* dstErr = (long*)SystemCallData->Argument[4];

    int processCount = gProcessManager->GetProcessCount();
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

    PCB* oldPcb = gProcessManager->GetPCBByName(name);
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

    EnterCriticalSection(0);
    gProcessManager->CreateProcess((char*)SystemCallData->Argument[0],
                                   1,
                                   (ProcessEntry)SystemCallData->Argument[1],
                                   (int)SystemCallData->Argument[2],
                                   (long*)SystemCallData->Argument[3],
                                   (long*)SystemCallData->Argument[4]);
    LeaveCriticalSection(0);
}
//****************************************************************************
void SVCStartTimer(SYSTEM_CALL_DATA* SystemCallData)
{
    INT32 currentTime;
    INT32 Status;
    INT32 sleepTime;

    EnterCriticalSection(0);

    // Find caller's PCB.
    PCB* pcb = gProcessManager->GetRunningProcess();

    CALL(MEM_READ(Z502ClockStatus, &currentTime));
    sleepTime = (INT32)SystemCallData->Argument[0];
    pcb->timerQueueKey = currentTime + sleepTime;

    // Check timer queue to see if there is a process to be awakened earlier.
    int needRestartTimer = 1;
    if( gProcessManager->GetTimerQueueProcessCount() > 0 )
    {
        PCB* anotherSleepingProcess = gProcessManager->GetTimerQueueProcess(0);
        if( pcb->timerQueueKey > anotherSleepingProcess->timerQueueKey )
        {
            needRestartTimer = 0;
        }
    }

    gProcessManager->PushToTimerQueue(pcb);

    MEM_READ(Z502TimerStatus, &Status);
    //if( Status == DEVICE_FREE )
    //{
    //    printf("SVCStartTimer: Timer is free\n");
    //}
    //else
    //{
    //    printf("SVCStartTimer: Timer is busy\n");
    //}

    if( needRestartTimer == 1 )
    {
        MEM_WRITE(Z502TimerStart, &sleepTime);

        MEM_READ(Z502TimerStatus, &Status);
        //if( Status == DEVICE_FREE )
        //{
        //    printf("SVCStartTimer: Timer is free\n");
        //}
        //else
        //{
        //    printf("SVCStartTimer: Timer is busy\n");
        //}
    }

    LeaveCriticalSection(0);

    gScheduler->OnProcessSleep();
}
//****************************************************************************