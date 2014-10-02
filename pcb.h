#ifndef PCB_H_
#define PCB_H_

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
    void*         context;
} PCB;

#endif