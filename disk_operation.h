#ifndef DISK_OPERATION_H_
#define DISK_OPERATION_H_

#include "pcb.h"

typedef struct DiskOperation
{
    long diskID;
    long sector;
    int  isRead;
    char* buffer;
    PCB*  requester;

} DiskOperation;

#endif