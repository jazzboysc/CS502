#ifndef PROCESS_MANAGER_H_
#define PROCESS_MANAGER_H_

#include "pcb.h"

void ProcessManagerInit();

int GetProcessCount();
PCB* GetPCBByID(long processID);
PCB* GetPCBByName(char* name);
PCB* GetPCBByContext(void* context);

void   RemovePCBFromRunningListByID(long processID);
void   RemoveFromTimerQueueByID(long processID);
void   RemoveFromReadyQueueByID(long processID);
void   PopFromTimerQueue(PCB** ppcb);
void   PushToTimerQueue(PCB* pcb);
void   PopFromReadyQueue(PCB** ppcb);
void   PushToReadyQueue(PCB* pcb);
PCB*   OSCreateProcess(char* name, ProcessEntry entry, int priority, long* reg1, long* reg2);

#endif