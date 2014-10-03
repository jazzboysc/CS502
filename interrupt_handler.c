#include "interrupt_handler.h"
#include "process_manager.h"
#include "scheduler.h"
#include "os_common.h"

//****************************************************************************
void IHTimerInterrupt()
{
    int timerQueueProcessCount = gProcessManager->GetTimerQueueProcessCount();
    if( timerQueueProcessCount == 0 )
    {
        // Something is wrong. The process who set the timer has been removed. 
        assert(0);
        return;
    }

    PCB* schedulerPCB = NULL;
    PCB* pcb = NULL;
    PCB* otherPCB = NULL;

    // Wake the first process in timer queue and see if it is the scheduler.
    gProcessManager->PopFromTimerQueue(&pcb);
    if( pcb->type == 0 )
    {
        schedulerPCB = pcb;
    }
    else
    {
        // Only push user process to ready queue.
        gProcessManager->PushToReadyQueue(pcb);
    }
    printf("Waking process %d\n", pcb->processID);

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

        if( otherPCB->type == 0 )
        {
            schedulerPCB = otherPCB;
        }
        else
        {
            // Only push user process to ready queue.
            gProcessManager->PushToReadyQueue(otherPCB);
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

    if( schedulerPCB )
    {
        Z502SwitchContext(SWITCH_CONTEXT_SAVE_MODE, &schedulerPCB->context);
    }
}
//****************************************************************************