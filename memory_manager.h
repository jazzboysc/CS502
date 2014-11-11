//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#ifndef MEMORY_MANAGER_H_
#define MEMORY_MANAGER_H_

#include "os_common.h"

typedef UINT16 (*MemoryManagerAllocPhysicalMemory)();

// Memory manager is a global singleton object used to manage physical memory
// of Z502 machine.
typedef struct MemoryManager
{
    MemoryManagerAllocPhysicalMemory AllocPhysicalMemory;

} MemoryManager;

// Create memory manager when the OS boots.
void MemoryManagerInitialize();
void MemoryManagerTerminate();

// Memory manager global singleton object.
extern MemoryManager* gMemoryManager;

#endif