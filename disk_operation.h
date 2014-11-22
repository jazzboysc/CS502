#ifndef DISK_OPERATION_H_
#define DISK_OPERATION_H_

#include "pcb.h"

#define DISK_OP_READ        0
#define DISK_OP_WRITE       1
#define DISK_OP_READ_CACHE  2
#define DISK_OP_WRITE_CACHE 3

typedef struct DiskOperation
{
    long diskID;
    long sector;
    int  operation;
    char* buffer;
    PCB*  requester;

} DiskOperation;

#endif