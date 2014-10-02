#include "scheduler.h"
#include "os_common.h"
#include "process_manager.h"

Scheduler* gScheduler;

//****************************************************************************
void Dispatch()
{
    while( gProcessManager->GetProcessCount() > 1 )
    {
        printf("User process exists. Dispatching... \n");
        // Dispatch user process.

    }
}
//****************************************************************************

//****************************************************************************
void SchedulerInitialize()
{
    gScheduler = ALLOC(Scheduler);
    gScheduler->Dispatch = Dispatch;
}
//****************************************************************************
void SchedulerTerminate()
{
    DEALLOC(gScheduler);
}
//****************************************************************************