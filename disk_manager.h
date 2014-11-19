//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#ifndef DISK_MANAGER_H_
#define DISK_MANAGER_H_

#include "pcb.h"
#include "disk_operation.h"

typedef void(*ProcessManagerPushToDiskOperationToDoList)(DiskOperation* diskOp);
typedef void(*ProcessManagerPopFromDiskOperationToDoList)(int diskID, DiskOperation** diskOp);
typedef void(*ProcessManagerPushToDiskOperationWaitList)(DiskOperation* diskOp);
typedef void(*ProcessManagerPopFromDiskOperationWaitList)(int diskID, DiskOperation** diskOp);

// Disk manager is a global singleton object used to manage disk operation
// of Z502 machine.
typedef struct DiskManager
{
    // Disk operation interfaces.
    ProcessManagerPushToDiskOperationToDoList  PushToDiskOperationToDoList;
    ProcessManagerPopFromDiskOperationToDoList PopFromDiskOperationToDoList;
    ProcessManagerPushToDiskOperationWaitList  PushToDiskOperationWaitList;
    ProcessManagerPopFromDiskOperationWaitList PopFromDiskOperationWaitList;

} DiskManager;

// Create disk manager when the OS boots.
void DiskManagerInitialize();
void DiskManagerTerminate();

// Disk manager global singleton object.
extern DiskManager* gDiskManager;

#endif