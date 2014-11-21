//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#ifndef MEMORY_MANAGER_H_
#define MEMORY_MANAGER_H_

#include "os_common.h"
#include "pcb.h"

typedef UINT16 (*MemoryManagerMapPhysicalMemory)(INT32 virtualPageNumber);
typedef void (*MemoryManagerSwapOut)(INT32 physicalPageNumber, PCB* user, 
    INT32* dstDiskID, INT32* dstSector);
typedef void (*MemoryManagerSwapIn)(INT32 physicalPageNumber, PCB* user,
    INT32 srcDiskID, INT32 srcSector);

// Memory manager is a global singleton object used to manage physical memory
// of Z502 machine.
typedef struct MemoryManager
{
    MemoryManagerMapPhysicalMemory MapPhysicalMemory;
    MemoryManagerSwapOut           SwapOut;
    MemoryManagerSwapIn            SwapIn;

} MemoryManager;

// Create memory manager when the OS boots.
void MemoryManagerInitialize();
void MemoryManagerTerminate();

// Memory manager global singleton object.
extern MemoryManager* gMemoryManager;

#endif