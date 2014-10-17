//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#include "scheduler.h"
#include "os_common.h"
#include "process_manager.h"
#include "critical_section.h"

Scheduler* gScheduler;

//****************************************************************************
void MakeReadyToRun()
{
    PCB* pcb = NULL;
    pcb = gProcessManager->GetReadyQueueProcess(0);
    if( pcb )
    {
        if( pcb->state == PROCESS_STATE_DEAD )
        {
            assert(0);
        }

        if( pcb->readyQueueKey > LEGAL_PRIORITY_MAX )
        {
            gProcessManager->ResetReadyQueueKeys();
        }

        gProcessManager->PopFromReadyQueue(&pcb);
        gProcessManager->SetRunningProcess(pcb);
        pcb->readyQueueKey += pcb->priority;

#ifdef PRINT_STATE
        gProcessManager->PrintState();
#endif

        LeaveCriticalSection(0);
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
        LeaveCriticalSection(0);
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
        LeaveCriticalSection(0);
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
        LeaveCriticalSection(0);
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