//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#ifndef DISK_SECTOR_INFO_H_
#define DISK_SECTOR_INFO_H_

#include "os_common.h"
#include "pcb.h"

// Process message.
typedef struct _DiskSectorInfo
{
    PCB* user;
    int usage;

} DiskSectorInfo;

#endif