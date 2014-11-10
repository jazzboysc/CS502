//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#include "interrupt_handler.h"
#include "process_manager.h"
#include "scheduler.h"
#include "os_common.h"
#include "critical_section.h"

//****************************************************************************
void FHMemoryFault(INT32 pageNumber)
{
    Z502_PAGE_TBL_LENGTH = VIRTUAL_MEM_PAGES;
    Z502_PAGE_TBL_ADDR = (UINT16 *)calloc(sizeof(UINT16),
        Z502_PAGE_TBL_LENGTH);

    Z502_PAGE_TBL_ADDR[pageNumber] = (UINT16)PTBL_VALID_BIT;
}
//****************************************************************************