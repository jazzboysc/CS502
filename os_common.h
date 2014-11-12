//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

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

#define LEGAL_PRIORITY_MIN 1
#define LEGAL_PRIORITY_MAX 200
#define LEGAL_MESSAGE_LENGTH_MAX 256
#define MAX_MESSAGE_LIST_NUM 16

// SVC errors.
#define MAX_PROCESS_NUM 9
#define ERR_CREAT_PROCESS_ILLEGAL_PRIORITY  22L
#define ERR_CREAT_PROCESS_ILLEGAL_NAME      23L
#define ERR_CREAT_PROCESS_ILLEGAL_ENTRY     24L
#define ERR_CREAT_PROCESS_REACH_MAX_NUM     25L
#define ERR_PROCESS_ID_NOT_FOUND            30L
#define ERR_PROCESS_ALREADY_SUSPENDED       35L
#define ERR_PROCESS_ALREADY_RESUMED         36L
#define ERR_CHANGE_PROCESS_ILLEGAL_PRIORITY 40L
#define ERR_ILEGAL_MESSAGE_LENGTH           45L
#define ERR_DST_BUFFER_TOO_SMALL            46L
#define ERR_REACH_MAX_MSG_COUNT             47L

extern UINT16 *Z502_PAGE_TBL_ADDR;
extern INT16 Z502_PAGE_TBL_LENGTH;

// Print out process queues and lists state.
//#define PRINT_STATE

#endif