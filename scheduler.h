//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "pcb.h"

// Scheduler function pointers.
typedef void (*SchedulerOnRedispatch)();
typedef void (*SchedulerOnProcessTerminate)();
typedef void (*SchedulerOnProcessSleep)();
typedef void (*SchedulerOnProcessSuspend)();
typedef void (*SchedulerDispatch)();

typedef struct Scheduler
{
    SchedulerOnRedispatch       OnRedispatch;
    SchedulerOnProcessTerminate OnProcessTerminate;
    SchedulerOnProcessSleep     OnProcessSleep;
    SchedulerOnProcessSuspend   OnProcessSuspend;
    SchedulerDispatch           Dispatch;

    PCB* schedulerPCB;
} Scheduler;

// Create scheduler when the OS boots.
void SchedulerInitialize();
void SchedulerTerminate();

// Scheduler global singleton object.
extern Scheduler* gScheduler;

#endif