#include "svc.h"
#include "pcb.h"
#include "message.h"
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
    int terminateMyself = 0;

    EnterCriticalSection(0);

    if( (INT32)SystemCallData->Argument[0] == -2 )
    {
        LeaveCriticalSection(0);

        // Terminate all.
        Z502Halt();
    }
    else if( (INT32)SystemCallData->Argument[0] == -1 )
    {
        // Terminate myself.
        terminateMyself = 1;

        // Find caller's PCB.
        PCB* pcb = gProcessManager->GetRunningProcess();
        long processID = pcb->processID;

        gProcessManager->TerminateProcess(pcb);

        if( processID == 1 )
        {
            LeaveCriticalSection(0);

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

            LeaveCriticalSection(0);
            return;
        }

        if( pcb == gProcessManager->GetRunningProcess() )
        {
            // Terminate myself.
            terminateMyself = 1;
        }

        gProcessManager->TerminateProcess(pcb);
    }

    if( SystemCallData->Argument[1] )
    {
        *(long*)SystemCallData->Argument[1] = ERR_SUCCESS;
    }

    LeaveCriticalSection(0);

    if( terminateMyself == 1 )
    {
        gScheduler->OnProcessTerminate();
    }
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

    if( (INT32)SystemCallData->Argument[2] < LEGAL_PRIORITY_MIN ||
        (INT32)SystemCallData->Argument[2] > LEGAL_PRIORITY_MAX )
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

    if( needRestartTimer == 1 )
    {
        MEM_WRITE(Z502TimerStart, &sleepTime);

        MEM_READ(Z502TimerStatus, &Status);
    }

    LeaveCriticalSection(0);

    gScheduler->OnProcessSleep();
}
//****************************************************************************
void SVCSuspendProcess(SYSTEM_CALL_DATA* SystemCallData)
{
    EnterCriticalSection(0);

    long processID = (long)SystemCallData->Argument[0];
    PCB* pcb = gProcessManager->GetPCBByID(processID);

    if( !pcb || pcb->state == PROCESS_STATE_DEAD )
    {
        // Process doesn't exist or already terminated.
        *(long*)SystemCallData->Argument[1] =
            ERR_PROCESS_ID_NOT_FOUND;
        return;
    }

    if( pcb->state == PROCESS_STATE_SUSPENDED )
    {
        // Process is alread suspended.
        *(long*)SystemCallData->Argument[1] =
            ERR_PROCESS_ALREADY_SUSPENDED;

        LeaveCriticalSection(0);
        return;
    }

    if( pcb->state == PROCESS_STATE_SLEEPING )
    {
        // Process is sleeping, delay suspension.
        pcb->state = PROCESS_STATE_SUSPENDING;
        *(long*)SystemCallData->Argument[1] = ERR_SUCCESS;

        LeaveCriticalSection(0);
        return;
    }

    if( pcb->state == PROCESS_STATE_READY )
    {
        gProcessManager->RemoveFromReadyQueueByID(pcb->processID);
        gProcessManager->AddToSuspendedList(pcb);
        *(long*)SystemCallData->Argument[1] = ERR_SUCCESS;
    }

    LeaveCriticalSection(0);
}
//****************************************************************************
void SVCResumeProcess(SYSTEM_CALL_DATA* SystemCallData)
{
    EnterCriticalSection(0);

    long processID = (long)SystemCallData->Argument[0];
    PCB* pcb = gProcessManager->GetPCBByID(processID);

    if( !pcb || pcb->state == PROCESS_STATE_DEAD )
    {
        *(long*)SystemCallData->Argument[1] =
            ERR_PROCESS_ID_NOT_FOUND;

        LeaveCriticalSection(0);
        return;
    }

    if( pcb->state != PROCESS_STATE_SUSPENDED &&
        pcb->state != PROCESS_STATE_SUSPENDING )
    {
        *(long*)SystemCallData->Argument[1] =
            ERR_PROCESS_ALREADY_RESUMED;

        LeaveCriticalSection(0);
        return;
    }

    if( pcb->state == PROCESS_STATE_SUSPENDING )
    {
        pcb->state = PROCESS_STATE_SLEEPING;
        *(long*)SystemCallData->Argument[1] = ERR_SUCCESS;
    }
    else
    {
        gProcessManager->RemoveFromSuspendedListByID(pcb->processID);
        gProcessManager->PushToReadyQueue(pcb);
        *(long*)SystemCallData->Argument[1] = ERR_SUCCESS;
    }

    LeaveCriticalSection(0);
}
//****************************************************************************
void SVCChangeProcessPriority(SYSTEM_CALL_DATA* SystemCallData)
{
    EnterCriticalSection(0);

    if( (INT32)SystemCallData->Argument[1] < LEGAL_PRIORITY_MIN ||
        (INT32)SystemCallData->Argument[1] > LEGAL_PRIORITY_MAX )
    {
        *(long*)SystemCallData->Argument[2] =
            ERR_CHANGE_PROCESS_ILLEGAL_PRIORITY;

        LeaveCriticalSection(0);
        return;
    }

    long processID = (long)SystemCallData->Argument[0];
    PCB* pcb = NULL;
    if( processID == -1 )
    {
        pcb = gProcessManager->GetRunningProcess();
    }
    else
    {
        pcb = gProcessManager->GetPCBByID(processID);
    }

    if( !pcb || pcb->state == PROCESS_STATE_DEAD )
    {
        *(long*)SystemCallData->Argument[2] =
            ERR_PROCESS_ID_NOT_FOUND;

        LeaveCriticalSection(0);
        return;
    }

    pcb->priority = (INT32)SystemCallData->Argument[1];
    pcb->readyQueueKey = pcb->priority;
    if( pcb->state == PROCESS_STATE_READY )
    {
        gProcessManager->RemoveFromReadyQueueByID(pcb->processID);
        gProcessManager->PushToReadyQueue(pcb);
    }
    else
    {
        int iStop = 0;
    }

    *(long*)SystemCallData->Argument[2] = ERR_SUCCESS;
    LeaveCriticalSection(0);
}
//****************************************************************************
void SVCSendMessage(SYSTEM_CALL_DATA* SystemCallData)
{
    EnterCriticalSection(0);

    long processID = (long)SystemCallData->Argument[0];
    PCB* receiver = NULL;
    if( processID == -1 )
    {
        receiver = gProcessManager->GetRunningProcess();
    }
    else
    {
        receiver = gProcessManager->GetPCBByID(processID);
    }

    if( !receiver || receiver->state == PROCESS_STATE_DEAD )
    {
        *(long*)SystemCallData->Argument[3] =
            ERR_PROCESS_ID_NOT_FOUND;

        LeaveCriticalSection(0);
        return;
    }

    int msgCount = gProcessManager->GetMessageListCount(receiver);
    if( msgCount >= MAX_MESSAGE_LIST_NUM )
    {
        *(long*)SystemCallData->Argument[3] =
            ERR_REACH_MAX_MSG_COUNT;

        LeaveCriticalSection(0);
        return;
    }

    if( (INT32)SystemCallData->Argument[2] > LEGAL_MESSAGE_LENGTH_MAX )
    {
        *(long*)SystemCallData->Argument[3] =
            ERR_ILEGAL_MESSAGE_LENGTH;

        LeaveCriticalSection(0);
        return;
    }

    char* msg = (char*)SystemCallData->Argument[1];
    Message* m = ALLOC(Message);
    m->senderProcessID = gProcessManager->GetRunningProcess()->processID;
    m->length = strlen(msg);
    strcpy(m->buffer, msg);

    gProcessManager->AddMessage(receiver, m);

    *(long*)SystemCallData->Argument[3] = ERR_SUCCESS;
    LeaveCriticalSection(0);
}
//****************************************************************************
void SVCReceiveMessage(SYSTEM_CALL_DATA* SystemCallData)
{
    EnterCriticalSection(0);

    if( (INT32)SystemCallData->Argument[2] > LEGAL_MESSAGE_LENGTH_MAX )
    {
        *(long*)SystemCallData->Argument[5] =
            ERR_ILEGAL_MESSAGE_LENGTH;

        LeaveCriticalSection(0);
        return;
    }

    long processID = (long)SystemCallData->Argument[0];
    PCB* sender = NULL;
    if( processID == -1 )
    {
        sender = gProcessManager->GetRunningProcess();
    }
    else
    {
        sender = gProcessManager->GetPCBByID(processID);
    }

    if( !sender )
    {
        *(long*)SystemCallData->Argument[5] =
            ERR_PROCESS_ID_NOT_FOUND;

        LeaveCriticalSection(0);
        return;
    }

    PCB* receiver = gProcessManager->GetRunningProcess();
    Message* msg = gProcessManager->RemoveMessageBySenderID(receiver, 
        sender->processID);

    if( (INT32)SystemCallData->Argument[2] < msg->length )
    {
        *(long*)SystemCallData->Argument[5] =
            ERR_DST_BUFFER_TOO_SMALL;

        gProcessManager->AddMessage(receiver, msg);
        LeaveCriticalSection(0);
        return;
    }

    strcpy((char*)SystemCallData->Argument[1], msg->buffer);
    *(long*)SystemCallData->Argument[3] = msg->length;
    *(long*)SystemCallData->Argument[4] = msg->senderProcessID;
    *(long*)SystemCallData->Argument[5] = ERR_SUCCESS;

    LeaveCriticalSection(0);
}
//****************************************************************************