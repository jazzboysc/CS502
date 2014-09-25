#include "interrupt_handler.h"
#include "process_manager.h"
#include "os_common.h"

extern MinPriQueue* TimerQueue;

//****************************************************************************
void IHTimerInterrupt()
{
    PCB* pcb = 0;
    PopFromTimerQueue(&pcb);

    // Check timer queue to see if there is another sleeping process need to
    // be awakened.
    if( TimerQueue->heap.size > 0 )
    {
        INT32 currentTime;
        CALL(MEM_READ(Z502ClockStatus, &currentTime));

        HeapItem* anotherSleepingProcess = MinPriQueueGetMin(TimerQueue);
        INT32 deltaTime = anotherSleepingProcess->key.key - currentTime;
        INT32 Status;

        CALL(MEM_WRITE(Z502TimerStart, &deltaTime));
        CALL(MEM_READ(Z502TimerStatus, &Status));
        if( Status == DEVICE_IN_USE )
        {
            printf("IHTimerInterrupt: Timer restarted\n");
        }
        else
        {
            printf("IHTimerInterrupt: Unable to restart timer\n");
        }
    }

    PushToReadyQueue(pcb);
}
//****************************************************************************