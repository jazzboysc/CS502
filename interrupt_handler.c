#include "interrupt_handler.h"
#include "process_manager.h"
#include "os_common.h"

//****************************************************************************
void IHTimerInterrupt()
{
    PCB* pcb = 0;
    PopFromTimerQueue(&pcb);
    PushToReadyQueue(pcb);
}
//****************************************************************************