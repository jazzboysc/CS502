#ifndef OS_STRUCTS_H_
#define OS_STRUCTS_H_

typedef void(*ProcessEntry)(void);

typedef struct _PCB
{
    char*         name;
    ProcessEntry  entry;
    int           priority;
    int           timerQueueKey;
    int           readyQueueKey;
    void*         context;
} PCB;

#endif