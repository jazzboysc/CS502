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
void OnRedispatch(void)
{
    printf("User process ends. No other uers process is ready. Switch to scheduler... \n");
    while( gProcessManager->GetTimerQueueProcessCount() > 0 )
    {
        printf("User process exists. Waiting... \n");
    }
    Z502Halt();

    //while( gScheduler->schedulerPCB->state == PROCESS_STATE_SLEEP );

    //while( gProcessManager->GetProcessCount() > 1 )
    //{
    //    printf("User process exists. Dispatching... \n");
    //    // Dispatch user process.

    //}
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
        Z502SwitchContext(SWITCH_CONTEXT_SAVE_MODE, &gScheduler->schedulerPCB->context);
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
        //Z502SwitchContext(SWITCH_CONTEXT_SAVE_MODE, &gScheduler->schedulerPCB->context);
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
    gScheduler->OnRedispatch = OnRedispatch;
    gScheduler->OnProcessSleep = OnProcessSleep;
    gScheduler->OnProcessTerminate = OnProcessTerminate;
    gScheduler->Dispatch = Dispatch;

    gScheduler->schedulerPCB = 
        gProcessManager->CreateProcess("Scheduler", 0, OnRedispatch, 20, 0, 0);
}
//****************************************************************************
void SchedulerTerminate()
{
    DEALLOC(gScheduler);
}
//****************************************************************************