#include "scheduler.h"
#include "os_common.h"
#include "process_manager.h"

Scheduler* gScheduler;

//****************************************************************************
void MakeReadyToRun()
{
    PCB* pcb = NULL;
    gProcessManager->PopFromReadyQueue(&pcb);
    if( pcb )
    {
        gProcessManager->SetRunningProcess(pcb);
        Z502SwitchContext(SWITCH_CONTEXT_SAVE_MODE, &pcb->context);
    }
}
//****************************************************************************
void OnTimeOut()
{
    while( gProcessManager->GetProcessCount() > 1 )
    {
        printf("User process exists. Dispatching... \n");
        // Dispatch user process.

    }
}
//****************************************************************************
void OnProcessTerminate()
{
    if( gProcessManager->GetReadyQueueProcessCount() > 0 )
    {
        MakeReadyToRun();
    }
    else
    {
        Z502Idle();
    }
}
//****************************************************************************
void OnProcessSleep()
{
    if( gProcessManager->GetReadyQueueProcessCount() > 0 )
    {
        MakeReadyToRun();
    }
    else
    {
        Z502Idle();
    }
}
//****************************************************************************
void Dispatch()
{
    MakeReadyToRun();
}
//****************************************************************************

//****************************************************************************
void SchedulerInitialize()
{
    gScheduler = ALLOC(Scheduler);
    gScheduler->OnTimeOut = OnTimeOut;
    gScheduler->OnProcessSleep = OnProcessSleep;
    gScheduler->OnProcessTerminate = OnProcessTerminate;
    gScheduler->Dispatch = Dispatch;
}
//****************************************************************************
void SchedulerTerminate()
{
    DEALLOC(gScheduler);
}
//****************************************************************************