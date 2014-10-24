#ifndef PCB_H_
#define PCB_H_

#include "list.h"
#include "message.h"

// Process states.
#define PROCESS_STATE_UNKNOWN    0
#define PROCESS_STATE_READY      1
#define PROCESS_STATE_SLEEPING   2
#define PROCESS_STATE_RUNNING    3
#define PROCESS_STATE_SUSPENDED  4
#define PROCESS_STATE_SUSPENDING 5
#define PROCESS_STATE_DEAD       6

#define PROCESS_TYPE_SCHEDULER 0
#define PROCESS_TYPE_USER      1

// Process control block.
typedef void(*ProcessEntry)(void);
typedef struct _PCB
{
    char*         name;
    int           type; // 0 : scheduler process 1 : user process
    long          processID;
    ProcessEntry  entry;
    int           priority;
    int           timerQueueKey;
    int           readyQueueKey;
    int           state;
    void*         context;

    // Each process has a message list that is used to recieve messages.
    List*         messages;

} PCB;

#endif