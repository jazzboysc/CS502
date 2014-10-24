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
        // Process in ready queue shouln't be dead. This must be an error!
        if( pcb->state == PROCESS_STATE_DEAD )
        {
            assert(0);
        }

        // Check if a dispatching cycle has ended. If yes, restart a new cycle.
        if( pcb->readyQueueKey > LEGAL_PRIORITY_MAX )
        {
            gProcessManager->ResetReadyQueueKeys();
        }

        // Prepare to dispatch this process.
        gProcessManager->PopFromReadyQueue(&pcb);
        gProcessManager->SetRunningProcess(pcb);

        // Adjust dynamic priority.
        pcb->readyQueueKey += pcb->priority;

#ifdef PRINT_STATE
        gProcessManager->PrintState();
#endif

        // Dispatch.
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

        // No more active processes, halt the machine.
        if( gProcessManager->IsAllDead() == 1 )
        {
            Z502Halt();
        }

        // Dispatch a process that is ready.
        if( gProcessManager->GetReadyQueueProcessCount() > 0 )
        {
            MakeReadyToRun();
        }
    }
}
//****************************************************************************
void OnProcessTerminate()
{
    // Dispatch a process that is ready.
    if( gProcessManager->GetReadyQueueProcessCount() > 0 )
    {
        MakeReadyToRun();
    }
    else
    {
        // No process is ready, swtich to scheduler process.
        LeaveCriticalSection(0);
        Z502SwitchContext(SWITCH_CONTEXT_SAVE_MODE, 
            &gScheduler->schedulerPCB->context);
    }
}
//****************************************************************************
void OnProcessSleep()
{
    // Dispatch a process that is ready.
    if( gProcessManager->GetReadyQueueProcessCount() > 0 )
    {
        MakeReadyToRun();
    }
    else
    {
        // No process is ready, swtich to scheduler process.
        LeaveCriticalSection(0);
        Z502SwitchContext(SWITCH_CONTEXT_SAVE_MODE, 
            &gScheduler->schedulerPCB->context);
    }
}
//****************************************************************************
void OnProcessSuspend()
{
    // Dispatch a process that is ready.
    if( gProcessManager->GetReadyQueueProcessCount() > 0 )
    {
        MakeReadyToRun();
    }
    else
    {
        // No process is ready, swtich to scheduler process.
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
    // Create scheduler.
    gScheduler = (Scheduler*)ALLOC(Scheduler);
    gScheduler->OnRedispatch = OnRedispatch;
    gScheduler->OnProcessSleep = OnProcessSleep;
    gScheduler->OnProcessTerminate = OnProcessTerminate;
    gScheduler->OnProcessSuspend = OnProcessSuspend;
    gScheduler->Dispatch = Dispatch;

    // Create a system process for scheduler.
    gScheduler->schedulerPCB = 
        gProcessManager->CreateProcess("Scheduler", 0, OnRedispatch, 20, 0, 0);
}
//****************************************************************************
void SchedulerTerminate()
{
    DEALLOC(gScheduler);
}
//****************************************************************************