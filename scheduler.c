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
        if( pcb->state == PROCESS_STATE_DEAD )
        {
            assert(0);
        }

        gProcessManager->SetRunningProcess(pcb);
#ifdef PRINT_STATE
        gProcessManager->PrintState();
#endif
        Z502SwitchContext(SWITCH_CONTEXT_SAVE_MODE, &pcb->context);
    }
}
//****************************************************************************
void OnRedispatch(void)
{
    while( 1 )
    {
        INT32 Status;
        MEM_READ(Z502TimerStatus, &Status);

        if( gProcessManager->IsAllDead() == 1 )
        {
            Z502Halt();
        }

        if( gProcessManager->GetReadyQueueProcessCount() > 0 )
        {
            MakeReadyToRun();
        }
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
        Z502SwitchContext(SWITCH_CONTEXT_SAVE_MODE, 
            &gScheduler->schedulerPCB->context);
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
        Z502SwitchContext(SWITCH_CONTEXT_SAVE_MODE, 
            &gScheduler->schedulerPCB->context);
    }
}
//****************************************************************************
void OnProcessSuspend()
{
    if( gProcessManager->GetReadyQueueProcessCount() > 0 )
    {
        MakeReadyToRun();
    }
    else
    {
        Z502SwitchContext(SWITCH_CONTEXT_SAVE_MODE,
            &gScheduler->schedulerPCB->context);
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
    gScheduler = (Scheduler*)ALLOC(Scheduler);
    gScheduler->OnRedispatch = OnRedispatch;
    gScheduler->OnProcessSleep = OnProcessSleep;
    gScheduler->OnProcessTerminate = OnProcessTerminate;
    gScheduler->OnProcessSuspend = OnProcessSuspend;
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