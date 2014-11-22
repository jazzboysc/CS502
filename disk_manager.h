//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#ifndef DISK_MANAGER_H_
#define DISK_MANAGER_H_

#include "pcb.h"
#include "disk_operation.h"

typedef void (*DiskManagerPushToDiskOperationToDoList)(DiskOperation* diskOp);
typedef void (*DiskManagerPopFromDiskOperationToDoList)(int diskID, DiskOperation** diskOp);
typedef void (*DiskManagerPushToDiskOperationWaitList)(DiskOperation* diskOp);
typedef void (*DiskManagerPopFromDiskOperationWaitList)(int diskID, DiskOperation** diskOp);
typedef void (*DiskManagerGetDiskCache)(PCB* user, int* diskID, int* sector);
typedef void (*DiskManagerFreeDiskCache)(int diskID, int sector);

// Disk manager is a global singleton object used to manage disk operation
// of Z502 machine.
typedef struct DiskManager
{
    // Disk operation interfaces.
    DiskManagerPushToDiskOperationToDoList  PushToDiskOperationToDoList;
    DiskManagerPopFromDiskOperationToDoList PopFromDiskOperationToDoList;
    DiskManagerPushToDiskOperationWaitList  PushToDiskOperationWaitList;
    DiskManagerPopFromDiskOperationWaitList PopFromDiskOperationWaitList;
    DiskManagerGetDiskCache                 GetDiskCache;
    DiskManagerFreeDiskCache                FreeDiskCache;

} DiskManager;

// Create disk manager when the OS boots.
void DiskManagerInitialize();
void DiskManagerTerminate();

// Disk manager global singleton object.
extern DiskManager* gDiskManager;

#endif