#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "pcb.h"

typedef void (*SchedulerDispatch)();

typedef struct Scheduler
{
    SchedulerDispatch Dispatch;

    PCB* schedulerPCB;
} Scheduler;

void SchedulerInitialize();
void SchedulerTerminate();

extern Scheduler* gScheduler;

#endif