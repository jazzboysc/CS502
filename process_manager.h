#ifndef PROCESS_MANAGER_H_
#define PROCESS_MANAGER_H_

#include "pcb.h"

// Process manager function pointers.
typedef int  (*ProcessManagerGetProcessCount)();
typedef int  (*ProcessManagerGetTimerQueueProcessCount)();
typedef PCB* (*ProcessManagerGetTimerQueueProcess)(int i);
typedef int  (*ProcessManagerGetReadyQueueProcessCount)();
typedef PCB* (*ProcessManagerGetPCBByID)(long processID);
typedef PCB* (*ProcessManagerGetPCBByName)(char* name);
typedef PCB* (*ProcessManagerGetPCBByContext)(void* context);
typedef void (*ProcessManagerRemovePCBFromGlobalListByID)(long processID);
typedef void (*ProcessManagerRemoveFromTimerQueueByID)(long processID);
typedef void (*ProcessManagerRemoveFromReadyQueueByID)(long processID);
typedef void (*ProcessManagerPopFromTimerQueue)(PCB** ppcb);
typedef void (*ProcessManagerPushToTimerQueue)(PCB* pcb);
typedef void (*ProcessManagerPopFromReadyQueue)(PCB** ppcb);
typedef void (*ProcessManagerPushToReadyQueue)(PCB* pcb);
typedef PCB* (*ProcessManagerCreateProcess)(char* name, int type, ProcessEntry entry, int priority, long* reg1, long* reg2);
typedef void (*ProcessManagerTerminateAllProcess)();
typedef void (*ProcessManagerTerminateProcess)(PCB* pcb);
typedef void (*ProcessManagerSetRunningProcess)(PCB* pcb);
typedef PCB* (*ProcessManagerGetRunningProcess)();
typedef int  (*ProcessManagerIsAllDead)();
typedef void (*ProcessManagerAddToSuspendedList)(PCB* pcb);
typedef void (*ProcessManagerRemoveFromSuspendedListByID)(long processID);
typedef void (*ProcessManagerAddMessage)(PCB* pcb, Message* msg);
typedef Message* (*ProcessManagerRemoveMessageBySenderID)(PCB* pcb, long processID);
typedef Message* (*ProcessManagerGetMessageBySenderID)(PCB* pcb, long processID);
typedef int (*ProcessManagerGetMessageListCount)(PCB* pcb);
typedef int (*ProcessManagerBroadcastMessage)(long senderProcessID, Message* msg);
typedef Message* (*ProcessManagerGetFirstMessage)(PCB* pcb);
typedef void (*ProcessManagerPrintState)();

// Process manager is a global singleton object used to manage processes.
typedef struct ProcessManager
{
    ProcessManagerGetProcessCount              GetProcessCount;
    ProcessManagerGetTimerQueueProcessCount    GetTimerQueueProcessCount;
    ProcessManagerGetTimerQueueProcess         GetTimerQueueProcess;
    ProcessManagerGetReadyQueueProcessCount    GetReadyQueueProcessCount;
    ProcessManagerGetPCBByID                   GetPCBByID;
    ProcessManagerGetPCBByName                 GetPCBByName;
    ProcessManagerGetPCBByContext              GetPCBByContext;

    ProcessManagerRemovePCBFromGlobalListByID  RemovePCBFromGlobalListByID;
    ProcessManagerRemoveFromTimerQueueByID     RemoveFromTimerQueueByID;
    ProcessManagerRemoveFromReadyQueueByID     RemoveFromReadyQueueByID;
    ProcessManagerPopFromTimerQueue            PopFromTimerQueue;
    ProcessManagerPushToTimerQueue             PushToTimerQueue;
    ProcessManagerPopFromReadyQueue            PopFromReadyQueue;
    ProcessManagerPushToReadyQueue             PushToReadyQueue;
    ProcessManagerCreateProcess                CreateProcess;
    ProcessManagerTerminateAllProcess          TerminateAllProcess;
    ProcessManagerTerminateProcess             TerminateProcess;

    ProcessManagerSetRunningProcess            SetRunningProcess;
    ProcessManagerGetRunningProcess            GetRunningProcess;

    ProcessManagerIsAllDead                    IsAllDead;

    ProcessManagerAddToSuspendedList           AddToSuspendedList;
    ProcessManagerRemoveFromSuspendedListByID  RemoveFromSuspendedListByID;

    ProcessManagerAddMessage                   AddMessage;
    ProcessManagerRemoveMessageBySenderID      RemoveMessageBySenderID;
    ProcessManagerGetMessageBySenderID         GetMessageBySenderID;
    ProcessManagerGetMessageListCount          GetMessageListCount;
    ProcessManagerBroadcastMessage             BroadcastMessage;
    ProcessManagerGetFirstMessage              GetFirstMessage;

    ProcessManagerPrintState                   PrintState;

} ProcessManager;

void ProcessManagerInitialize();
void ProcessManagerTerminate();

extern ProcessManager* gProcessManager;

#endif