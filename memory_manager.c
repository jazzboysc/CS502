//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#include "memory_manager.h"
#include "process_manager.h"
#include "critical_section.h"
#include "disk_manager.h"

MemoryManager* gMemoryManager;

// Track the current status of a physical memory page.
typedef struct PhysicalPageStatus
{
    int   used;       // 0 : free 1 : used
    INT32 virtualPageNumber;
    PCB*  user;  // The process that is using this page

} PhysicalPageStatus;

PhysicalPageStatus gPhysicalPageStatusTable[PHYS_MEM_PGS];

//****************************************************************************
UINT16 MapPhysicalMemory(INT32 virtualPageNumber)
{
    EnterCriticalSection(1);

    // Check if we got a memory access crossing the boundary.
    if( virtualPageNumber >= VIRTUAL_MEM_PAGES )
    {
        LeaveCriticalSection(1);
        Z502Halt();
    }

    // The first time we encounter a memory fault, we need to create a
    // page table for the current process.
    if( Z502_PAGE_TBL_ADDR == NULL )
    {
        Z502_PAGE_TBL_LENGTH = VIRTUAL_MEM_PAGES;
        Z502_PAGE_TBL_ADDR = (UINT16 *)calloc(Z502_PAGE_TBL_LENGTH,
            sizeof(UINT16));
        gProcessManager->GetRunningProcess()->virtualPageTable = Z502_PAGE_TBL_ADDR;
    }

    // Try to find a free physical page first.
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

        // Choose a victim.
        i = 0;

        // Swap out data of the victim process.
        INT32 diskID, sector;
        gMemoryManager->SwapOut(i, gPhysicalPageStatusTable[i].user,
            &diskID, &sector);

        // Check if we need to swap in data for the current process.
        PCB* pcb = gProcessManager->GetRunningProcess();
        if( pcb->trackTable[virtualPageNumber].swappedOut )
        {
            // Swap in data of the running process.
            gMemoryManager->SwapIn(i, pcb,
                pcb->trackTable[virtualPageNumber].diskID,
                pcb->trackTable[virtualPageNumber].sector);
        }
    }
    gPhysicalPageStatusTable[i].used = 1;
    gPhysicalPageStatusTable[i].user = gProcessManager->GetRunningProcess();
    gPhysicalPageStatusTable[i].virtualPageNumber = virtualPageNumber;

    Z502_PAGE_TBL_ADDR[virtualPageNumber] = i | (UINT16)PTBL_VALID_BIT;

    LeaveCriticalSection(1);
    return i;
}
//****************************************************************************
void SwapOut(INT32 physicalPageNumber, PCB* user, INT32* dstDiskID, 
    INT32* dstSector)
{
    // Notify the previous user of the physical page that the page is no more
    // valid. Since we have to swap it out to save space for a new user.
    UINT16* virtualPageTable = user->virtualPageTable;
    INT32 vpn = gPhysicalPageStatusTable[physicalPageNumber].virtualPageNumber;
    virtualPageTable[vpn] &= (UINT16)0x7FFF;

    // Get data from physical memory.
    char tempBuffer[PGSIZE];
    Z502ReadPhysicalMemory(physicalPageNumber, tempBuffer);

    // Now swap out data for the old user.

    // Find a disk cache.
    gDiskManager->GetDiskCache(user, dstDiskID, dstSector);
    user->trackTable[vpn].diskID = *dstDiskID;
    user->trackTable[vpn].sector = *dstSector;
    user->trackTable[vpn].swappedOut = 1;

    // Wait untill disk is free.
    INT32 temp;
    MEM_WRITE(Z502DiskSetID, dstDiskID);
    MEM_READ(Z502DiskStatus, &temp);
    while( temp != DEVICE_FREE )
    {
        Z502Idle();
        MEM_READ(Z502DiskStatus, &temp);
    }

    DiskOperation* diskOp = ALLOC(DiskOperation);
    diskOp->requester = user;
    diskOp->operation = DISK_OP_WRITE_CACHE;
    diskOp->diskID = *dstDiskID;
    diskOp->sector = *dstSector;
    diskOp->buffer = tempBuffer;
    gDiskManager->PushToDiskOperationWaitList(diskOp);

    // Write data.
    MEM_WRITE(Z502DiskSetID, dstDiskID);
    MEM_WRITE(Z502DiskSetSector, dstSector);
    MEM_WRITE(Z502DiskSetBuffer, (INT32 *)tempBuffer);
    temp = 1;
    MEM_WRITE(Z502DiskSetAction, &temp);
    temp = 0;
    MEM_WRITE(Z502DiskStart, &temp);

    MEM_WRITE(Z502DiskSetID, dstDiskID);
    MEM_READ(Z502DiskStatus, &temp);
    while( temp != DEVICE_FREE )
    {
        Z502Idle();
        MEM_READ(Z502DiskStatus, &temp);
    }
}
//****************************************************************************
void SwapIn(INT32 physicalPageNumber, PCB* user, INT32 srcDiskID, 
    INT32 srcSector)
{

}
//****************************************************************************

//****************************************************************************
void MemoryManagerInitialize()
{
    // Create memory manager.
    gMemoryManager = (MemoryManager*)ALLOC(MemoryManager);
    gMemoryManager->MapPhysicalMemory = MapPhysicalMemory;
    gMemoryManager->SwapOut = SwapOut;
    gMemoryManager->SwapIn = SwapIn;

    // Initialize physical page status table.
    int i;
    for( i = 0; i < PHYS_MEM_PGS; ++i )
    {
        gPhysicalPageStatusTable[i].used = 0;
        gPhysicalPageStatusTable[i].user = 0;
        gPhysicalPageStatusTable[i].virtualPageNumber = -1;
    }
}
//****************************************************************************
void MemoryManagerTerminate()
{
    DEALLOC(gMemoryManager);
}
//****************************************************************************