#ifndef PCB_H_
#define PCB_H_

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