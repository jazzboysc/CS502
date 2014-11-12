//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#include "interrupt_handler.h"
#include "process_manager.h"
#include "scheduler.h"
#include "os_common.h"
#include "critical_section.h"

//****************************************************************************
void OnProcessWake(PCB* pcb)
{
    if( pcb->state == PROCESS_STATE_SLEEPING )
    {
        // Only push user process to ready queue.
        gProcessManager->PushToReadyQueue(pcb);
    }
    else if( pcb->state == PROCESS_STATE_SUSPENDING )
    {
        gProcessManager->AddToSuspendedList(pcb);
    }
    else
    {
        assert(pcb->state == PROCESS_STATE_DEAD);
    }
}
//****************************************************************************
void IHTimerInterrupt()
{
    EnterCriticalSection(1);

    int timerQueueProcessCount = gProcessManager->GetTimerQueueProcessCount();
    if( timerQueueProcessCount == 0 )
    {
        // Something is wrong. The process who set the timer has been removed. 
        //assert(0);
        LeaveCriticalSection(1);
        return;
    }

    PCB* schedulerPCB = NULL;
    PCB* pcb = NULL;
    PCB* otherPCB = NULL;

    // Wake the first process in timer queue and see if it is the scheduler.
    gProcessManager->PopFromTimerQueue(&pcb);
    if( pcb->type == PROCESS_TYPE_SCHEDULER )
    {
        schedulerPCB = pcb;
    }
    else
    {
        OnProcessWake(pcb);
    }

    // Check timer queue and see if there are other sleeping processes need to 
    // be awakened since multiple processes may want to wake at the same time.
    int i;
    int wakeCount = 0;
    for( i = 0; i < gProcessManager->GetTimerQueueProcessCount(); ++i )
    {
        otherPCB = gProcessManager->GetTimerQueueProcess(i);

        if( otherPCB->timerQueueKey == pcb->timerQueueKey )
        {
            wakeCount++;
        }
    }
    for( i = 0; i < wakeCount; ++i )
    {
        gProcessManager->PopFromTimerQueue(&otherPCB);

        if( otherPCB->type == PROCESS_TYPE_SCHEDULER )
        {
            schedulerPCB = otherPCB;
        }
        else
        {
            OnProcessWake(otherPCB);
        }
    }

    // Restart timer if necessary.
    if( gProcessManager->GetTimerQueueProcessCount() > 0 )
    {
        INT32 currentTime;
        MEM_READ(Z502ClockStatus, &currentTime);

        PCB* anotherSleepingProcess = gProcessManager->GetTimerQueueProcess(0);
        INT32 deltaTime = anotherSleepingProcess->timerQueueKey - currentTime;

        MEM_WRITE(Z502TimerStart, &deltaTime);
    }

    LeaveCriticalSection(1);

    if( schedulerPCB )
    {
        Z502SwitchContext(SWITCH_CONTEXT_SAVE_MODE, &schedulerPCB->context);
    }
}
//****************************************************************************
void IHDiskInterrupt()
{
    EnterCriticalSection(1);

    // Previous disk operation has finished, put the requester to ready queue.
    DiskOperation* diskOp;
    gProcessManager->PopFromDiskOperationWaitList(&diskOp);
    gProcessManager->PushToReadyQueue(diskOp->requester);
    DEALLOC(diskOp);

    // Start a new disk operation.
    gProcessManager->PopFromDiskOperationToDoList(&diskOp);
    if( diskOp != NULL )
    {
        INT32 temp;
        MEM_WRITE(Z502DiskSetID, &diskOp->diskID);
        MEM_WRITE(Z502DiskSetSector, &diskOp->sector);
        MEM_WRITE(Z502DiskSetBuffer, (INT32*)diskOp->buffer);
        MEM_WRITE(Z502DiskSetAction, &diskOp->isWrite);

        temp = 0;
        MEM_WRITE(Z502DiskStart, &temp);
    }

    LeaveCriticalSection(1);
}
//****************************************************************************