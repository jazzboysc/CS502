//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#include "memory_manager.h"
#include "process_manager.h"

MemoryManager* gMemoryManager;

// Track the current status of a physical memory page.
typedef struct PhysicalPageStatus
{
    int           used;       // 0 : free 1 : used
    long          processID;  // The process that is using this page

} PhysicalPageStatus;

PhysicalPageStatus gPhysicalPageStatusTable[PHYS_MEM_PGS];

//****************************************************************************
UINT16 AllocPhysicalMemory()
{
    UINT16 i;
    for( i = 0; i < (UINT16)PHYS_MEM_PGS; ++i )
    {
        if( gPhysicalPageStatusTable[i].used == 0 )
        {
            // A free physical page is found.
            break;
        }
    }

    if( i == PHYS_MEM_PGS )
    {
        // No free physical page is available.
        assert(0);
        return PHYS_MEM_PGS;
    }
    gPhysicalPageStatusTable[i].used = 1;
    gPhysicalPageStatusTable[i].processID = 
        gProcessManager->GetRunningProcess()->processID;

    return i;
}
//****************************************************************************

//****************************************************************************
void MemoryManagerInitialize()
{
    // Create memory manager.
    gMemoryManager = (MemoryManager*)ALLOC(MemoryManager);
    gMemoryManager->AllocPhysicalMemory = AllocPhysicalMemory;

    // Initialize physical page status table.
    int i;
    for( i = 0; i < PHYS_MEM_PGS; ++i )
    {
        gPhysicalPageStatusTable[i].used = 0;
        gPhysicalPageStatusTable[i].processID = 0;
    }
}
//****************************************************************************
void MemoryManagerTerminate()
{
    DEALLOC(gMemoryManager);
}
//****************************************************************************