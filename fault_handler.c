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
void FHMemoryFault(INT32 virtualPageNumber)
{
    gMemoryManager->MapPhysicalMemory(virtualPageNumber);
}
//****************************************************************************