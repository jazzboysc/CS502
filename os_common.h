#ifndef OS_COMMON_H_
#define OS_COMMON_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "syscalls.h"
#include "protos.h"
#include "string.h"
#include "z502.h"
#include "list.h"

#include "priority_queue.h"
#include "memory.h"

// These loacations are global and define information about the page table
extern UINT16*       Z502_PAGE_TBL_ADDR;
extern INT16         Z502_PAGE_TBL_LENGTH;
extern Z502CONTEXT*  Z502_CURRENT_CONTEXT;

#ifndef ILLEGAL_PRIORITY
#define ILLEGAL_PRIORITY  -3
#endif
#ifndef LEGAL_PRIORITY
#define LEGAL_PRIORITY    10
#endif

#define MAX_PROCESS_NUM 16
#define ERR_CREAT_PROCESS_ILLEGAL_PRIORITY  22L
#define ERR_CREAT_PROCESS_ILLEGAL_NAME      23L
#define ERR_CREAT_PROCESS_ILLEGAL_ENTRY     24L
#define ERR_CREAT_PROCESS_REACH_MAX_NUM     25L
#define ERR_PROCESS_ID_NOT_FOUND            30L
#define ERR_PROCESS_ALREADY_SUSPENDED       35L
#define ERR_PROCESS_ALREADY_RESUMED         36L

#endif