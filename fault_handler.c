//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#include "interrupt_handler.h"
#include "process_manager.h"
#include "memory_manager.h"
#include "scheduler.h"
#include "os_common.h"
#include "critical_section.h"

//****************************************************************************
void FHMemoryFault(INT32 pageNumber)
{
    // Check if we got a memory access crossing the boundary.
    if( pageNumber >= VIRTUAL_MEM_PAGES )
    {
        Z502Halt();
    }

    // The first time we encounter a memory fault, we need to create a
    // page table for the current process.
    if( Z502_PAGE_TBL_ADDR == NULL )
    {
        Z502_PAGE_TBL_LENGTH = VIRTUAL_MEM_PAGES;
        Z502_PAGE_TBL_ADDR = (UINT16 *)calloc(sizeof(UINT16),
            Z502_PAGE_TBL_LENGTH);
    }

    UINT16 physicalPageNumber = gMemoryManager->AllocPhysicalMemory();
    Z502_PAGE_TBL_ADDR[pageNumber] = physicalPageNumber | 
        (UINT16)PTBL_VALID_BIT;
}
//****************************************************************************