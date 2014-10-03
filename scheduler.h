#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "pcb.h"

typedef void (*SchedulerOnRedispatch)();
typedef void (*SchedulerOnProcessTerminate)();
typedef void (*SchedulerOnProcessSleep)();
typedef void (*SchedulerDispatch)();

typedef struct Scheduler
{
    SchedulerOnRedispatch       OnRedispatch;
    SchedulerOnProcessTerminate OnProcessTerminate;
    SchedulerOnProcessSleep     OnProcessSleep;
    SchedulerDispatch           Dispatch;

    PCB* schedulerPCB;
} Scheduler;

void SchedulerInitialize();
void SchedulerTerminate();

extern Scheduler* gScheduler;

#endif