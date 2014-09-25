#include "scheduler.h"
#include "os_common.h"

extern List* GlobalProcessList;
extern MinPriQueue* TimerQueue;
extern MinPriQueue* ReadyQueue;