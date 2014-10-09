#ifndef PCB_H_
#define PCB_H_

#define PROCESS_STATE_UNKNOWN 0
#define PROCESS_STATE_READY   1
#define PROCESS_STATE_SLEEP   2
#define PROCESS_STATE_RUNNING 3
#define PROCESS_STATE_DEAD    4

#define PROCESS_TYPE_SCHEDULER 0
#define PROCESS_TYPE_USER      1

typedef void(*ProcessEntry)(void);
typedef struct _PCB
{
    char*         name;
    int           type; // 0 : scheduler process 1 : user process
    long          processID;
    ProcessEntry  entry;
    int           priority;
    int           currentPriority;
    int           timerQueueKey;
    int           readyQueueKey;
    int           state; // 1 : ready 2 : sleep 3 : running
    void*         context;
} PCB;

#endif