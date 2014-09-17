#ifndef OS_COMMON_H_
#define OS_COMMON_H_

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
#define ERR_TERMINATE_PROCESS_ID_NOT_FOUND  30L

typedef void(*ProcessEntry)(void);

typedef struct _PCB
{
    char*         name;
    long          processID;
    ProcessEntry  entry;
    int           priority;
    int           timerQueueKey;
    int           readyQueueKey;
    void*         context;
} PCB;

#endif