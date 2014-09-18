/************************************************************************

        This code forms the base of the operating system you will
        build.  It has only the barest rudiments of what you will
        eventually construct; yet it contains the interfaces that
        allow test.c and z502.c to be successfully built together.

        Revision History:
        1.0 August 1990
        1.1 December 1990: Portability attempted.
        1.3 July     1992: More Portability enhancements.
                           Add call to sample_code.
        1.4 December 1992: Limit (temporarily) printout in
                           interrupt handler.  More portability.
        2.0 January  2000: A number of small changes.
        2.1 May      2001: Bug fixes and clear STAT_VECTOR
        2.2 July     2002: Make code appropriate for undergrads.
                           Default program start is in test0.
        3.0 August   2004: Modified to support memory mapped IO
        3.1 August   2004: hardware interrupt runs on separate thread
        3.11 August  2004: Support for OS level locking
	4.0  July    2013: Major portions rewritten to support multiple threads
************************************************************************/

#include             "global.h"
#include             "syscalls.h"
#include             "protos.h"
#include             "string.h"
#include             "z502.h"
#include             "list.h"
#include             "priority_queue.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// These loacations are global and define information about the page table
extern UINT16*       Z502_PAGE_TBL_ADDR;
extern INT16         Z502_PAGE_TBL_LENGTH;
extern Z502CONTEXT*  Z502_CURRENT_CONTEXT;

// OS global variables.
List* RunningList;
MinPriQueue* TimerQueue;
MinPriQueue* ReadyQueue;

extern void          *TO_VECTOR [];

char                 *call_names[] = { "mem_read ", "mem_write",
                            "read_mod ", "get_time ", "sleep    ",
                            "get_pid  ", "create   ", "term_proc",
                            "suspend  ", "resume   ", "ch_prior ",
                            "send     ", "receive  ", "disk_read",
                            "disk_wrt ", "def_sh_ar" };


/************************************************************************
    INTERRUPT_HANDLER
        When the Z502 gets a hardware interrupt, it transfers control to
        this routine in the OS.
************************************************************************/
void    interrupt_handler( void ) {
    INT32              device_id;
    INT32              status;
    INT32              Index = 0;

    // Get cause of interrupt
    MEM_READ(Z502InterruptDevice, &device_id );
    // Set this device as target of our query
    MEM_WRITE(Z502InterruptDevice, &device_id );
    // Now read the status of this device
    MEM_READ(Z502InterruptStatus, &status );

    switch( device_id )
    {
    case 4:
        TimerInterrupt();
        break;

    default:
        break;
    }

    // Clear out this device - we're done with it
    MEM_WRITE(Z502InterruptClear, &Index );
}                                       /* End of interrupt_handler */
/************************************************************************
    FAULT_HANDLER
        The beginning of the OS502.  Used to receive hardware faults.
************************************************************************/

void    fault_handler( void )
    {
    INT32       device_id;
    INT32       status;
    INT32       Index = 0;

    // Get cause of interrupt
    MEM_READ(Z502InterruptDevice, &device_id );
    // Set this device as target of our query
    MEM_WRITE(Z502InterruptDevice, &device_id );
    // Now read the status of this device
    MEM_READ(Z502InterruptStatus, &status );

    printf( "Fault_handler: Found vector type %d with value %d\n",
                        device_id, status );
    // Clear out this device - we're done with it
    MEM_WRITE(Z502InterruptClear, &Index );
}                                       /* End of fault_handler */

/************************************************************************
    SVC
        The beginning of the OS502.  Used to receive software interrupts.
        All system calls come to this point in the code and are to be
        handled by the student written code here.
        The variable do_print is designed to print out the data for the
        incoming calls, but does so only for the first ten calls.  This
        allows the user to see what's happening, but doesn't overwhelm
        with the amount of data.
************************************************************************/

void    svc( SYSTEM_CALL_DATA *SystemCallData ) {
    short               call_type;
    static short        do_print = 10;
    short               i;
    INT32               Time;

    call_type = (short)SystemCallData->SystemCallNumber;
    if ( do_print > 0 ) {
        printf( "SVC handler: %s\n", call_names[call_type]);
        for (i = 0; i < SystemCallData->NumberOfArguments - 1; i++ ){
        	 //Value = (long)*SystemCallData->Argument[i];
             printf( "Arg %d: Contents = (Decimal) %8ld,  (Hex) %8lX\n", i,
             (unsigned long )SystemCallData->Argument[i],
             (unsigned long )SystemCallData->Argument[i]);
        }
    do_print--;
    }

    switch (call_type) {
    case SYSNUM_GET_TIME_OF_DAY:
        CALL(MEM_READ(Z502ClockStatus, &Time));
        *(INT32*)SystemCallData->Argument[0] = Time;
        break;

    case SYSNUM_GET_PROCESS_ID:
        SVCGetProcessID(SystemCallData);
        break;

    case SYSNUM_CREATE_PROCESS:
        SVCCreateProcess(SystemCallData);
        break;

    case SYSNUM_SLEEP:
        SVCStartTimer(SystemCallData);
        break;

    case SYSNUM_TERMINATE_PROCESS:
        SVCTerminateProcess(SystemCallData);
        break;

    default:
        printf("ERROR!  call_type not recognized!\n");
        printf("Call_type is - %i\n", call_type);
        break;
    }
}                                               // End of svc



/************************************************************************
    osInit
        This is the first routine called after the simulation begins.  This
        is equivalent to boot code.  All the initial OS components can be
        defined and initialized here.
************************************************************************/

void    osInit( int argc, char *argv[]  ) {
    void                *next_context;
    INT32               i;

    /* Demonstrates how calling arguments are passed thru to here       */

    printf( "Program called with %d arguments:", argc );
    for ( i = 0; i < argc; i++ )
        printf( " %s", argv[i] );
    printf( "\n" );
    printf( "Calling with argument 'sample' executes the sample program.\n" );

    /*          Setup so handlers will come to code in base.c           */

    TO_VECTOR[TO_VECTOR_INT_HANDLER_ADDR]   = (void *)interrupt_handler;
    TO_VECTOR[TO_VECTOR_FAULT_HANDLER_ADDR] = (void *)fault_handler;
    TO_VECTOR[TO_VECTOR_TRAP_HANDLER_ADDR]  = (void *)svc;

    /*  Determine if the switch was set, and if so go to demo routine.  */

    if (( argc > 1 ) && ( strcmp( argv[1], "sample" ) == 0 ) ) {
        Z502MakeContext( &next_context, (void *)sample_code, KERNEL_MODE );
        Z502SwitchContext( SWITCH_CONTEXT_KILL_MODE, &next_context );
    }                   /* This routine should never return!!           */

    // Init OS global variables.
    RunningList = calloc(1, sizeof(List));
    TimerQueue = calloc(1, sizeof(MinPriQueue));
    MinPriQueueInit(TimerQueue, MAX_PROCESS_NUM);
    ReadyQueue = calloc(1, sizeof(MinPriQueue));
    MinPriQueueInit(ReadyQueue, MAX_PROCESS_NUM);

    OSCreateProcess("test1b", test1b, 10, 0, 0);
}                                               // End of osInit

//****************************************************************************
int GetProcessCount()
{
    if( RunningList )
    {
        return RunningList->count;
    }

    return 0;
}
//****************************************************************************
PCB* GetPCBByID(long processID)
{
    ListNode* currentNode = RunningList->head;
    while( currentNode )
    {
        if( ((PCB*)currentNode->data)->processID == processID )
        {
            return (PCB*)currentNode->data;
        }
        currentNode = currentNode->next;
    }

    return 0;
}
//****************************************************************************
PCB* GetPCBByName(char* name)
{
    ListNode* currentNode = RunningList->head;
    while( currentNode )
    {
        if( strcmp(((PCB*)currentNode->data)->name, name) == 0 )
        {
            return (PCB*)currentNode->data;
        }
        currentNode = currentNode->next;
    }

    return 0;
}
//****************************************************************************
PCB* GetPCBByContext(void* context)
{
    ListNode* currentNode = RunningList->head;
    while( currentNode )
    {
        if( ((PCB*)currentNode->data)->context == context )
        {
            return (PCB*)currentNode->data;
        }
        currentNode = currentNode->next;
    }

    return 0;
}
//****************************************************************************
void TimerInterrupt()
{
    PCB* pcb = 0;
    PopFromTimerQueue(&pcb);
    PushToReadyQueue(pcb);
}
//****************************************************************************
void SVCGetProcessID(SYSTEM_CALL_DATA* SystemCallData)
{
    size_t nameLen = strlen((char*)SystemCallData->Argument[0]);
    if( nameLen == 0 )
    {
        // Find caller's PCB.
        void* currentContext = (void*)Z502_CURRENT_CONTEXT;
        PCB* pcb = GetPCBByContext(currentContext);

        long processID = pcb->processID;
        *(long*)SystemCallData->Argument[1] = processID;
        *(long*)SystemCallData->Argument[2] = ERR_SUCCESS;
    }
    else
    {
        PCB* pcb = GetPCBByName((char*)SystemCallData->Argument[0]);
        if( pcb )
        {
            long processID = pcb->processID;
            *(long*)SystemCallData->Argument[1] = processID;
            *(long*)SystemCallData->Argument[2] = ERR_SUCCESS;
        }
        else
        {
            *(long*)SystemCallData->Argument[2] = ERR_PROCESS_ID_NOT_FOUND;
        }
    }
}
//****************************************************************************
void SVCTerminateProcess(SYSTEM_CALL_DATA* SystemCallData)
{
    if( (INT32)SystemCallData->Argument[0] == -2 )
    {
        // Terminate all.

        ListNode* currentProcessNode = RunningList->head;
        for( int i = 0; i < RunningList->count; ++i )
        {
            PCB* pcb = (PCB*)currentProcessNode->data;

            RemoveFromTimerQueueByID(pcb->processID);
            RemoveFromReadyQueueByID(pcb->processID);

            free(pcb);
        }

        ListRelease(RunningList);
        Z502Halt();
    }
    else if( (INT32)SystemCallData->Argument[0] == -1 )
    {
        // Terminate myself.

        // Find caller's PCB.
        void* currentContext = (void*)Z502_CURRENT_CONTEXT;
        PCB* pcb = GetPCBByContext(currentContext);
        long processID = pcb->processID;

        // Remove from global list and queues.
        RemoveFromRunningListByID(processID);
        RemoveFromTimerQueueByID(processID);
        RemoveFromReadyQueueByID(processID);

        free(pcb);

        if( processID == 1 )
        {
            Z502Halt();
        }
    }
    else
    {
        // Terminate by id.

        long processID = *(long*)SystemCallData->Argument[0];
        PCB* pcb = GetPCBByID(processID);

        if( !pcb )
        {
            *(long*)SystemCallData->Argument[1] =
                ERR_PROCESS_ID_NOT_FOUND;
            return;
        }

        // Remove from global list and queues.
        RemoveFromRunningListByID(processID);
        RemoveFromTimerQueueByID(processID);
        RemoveFromReadyQueueByID(processID);

        //Z502DestroyContext(&pcb->context);
        free(pcb);
    }

    if( SystemCallData->Argument[1] )
    {
        *(long*)SystemCallData->Argument[1] = ERR_SUCCESS;
    }
}
//****************************************************************************
void SVCCreateProcess(SYSTEM_CALL_DATA* SystemCallData)
{
    long* dstErr = (long*)SystemCallData->Argument[4];

    int processCount = GetProcessCount();
    if( processCount > MAX_PROCESS_NUM )
    {
        if( dstErr )
        {
            *dstErr = ERR_CREAT_PROCESS_REACH_MAX_NUM;
        }
        return;
    }

    if( (INT32)SystemCallData->Argument[2] == ILLEGAL_PRIORITY )
    {
        if( dstErr )
        {
            *dstErr = ERR_CREAT_PROCESS_ILLEGAL_PRIORITY;
        }
        return;
    }

    char* name = (char*)SystemCallData->Argument[0];
    if( !name )
    {
        if( dstErr )
        {
            *dstErr = ERR_CREAT_PROCESS_ILLEGAL_NAME;
        }
        return;
    }

    PCB* oldPcb = GetPCBByName(name);
    if( oldPcb )
    {
        if( dstErr )
        {
            *dstErr = ERR_CREAT_PROCESS_ILLEGAL_NAME;
        }
        return;
    }

    ProcessEntry entry = (ProcessEntry)SystemCallData->Argument[1];
    if( !entry )
    {
        if( dstErr )
        {
            *dstErr = ERR_CREAT_PROCESS_ILLEGAL_ENTRY;
        }
        return;
    }

    OSCreateProcess((char*)SystemCallData->Argument[0],
                    (ProcessEntry)SystemCallData->Argument[1],
                    (int)SystemCallData->Argument[2],
                    (long*)SystemCallData->Argument[3],
                    (long*)SystemCallData->Argument[4]);
}
//****************************************************************************
void SVCStartTimer(SYSTEM_CALL_DATA* SystemCallData)
{
    INT32               currentTime;
    INT32               Status;
    INT32               sleepTime;
    void*               currentContext;

    // Find caller's PCB.
    currentContext = (void*)Z502_CURRENT_CONTEXT;
    PCB* pcb = GetPCBByContext(currentContext);

    CALL(MEM_READ(Z502ClockStatus, &currentTime));
    sleepTime = (INT32)SystemCallData->Argument[0];
    pcb->timerQueueKey = currentTime + sleepTime;

    RemoveFromReadyQueueByID(pcb->processID);
    PushToTimerQueue(pcb);

    CALL(MEM_READ(Z502TimerStatus, &Status));
    if( Status == DEVICE_FREE )
    {
        printf("Timer is free\n");
    }
    else
    {
        printf("Timer is busy\n");
    }

    CALL(MEM_WRITE(Z502TimerStart, &sleepTime));
    CALL(MEM_READ(Z502TimerStatus, &Status));
    if( Status == DEVICE_IN_USE )
    {
        printf("Timer started\n");
    }
    else
    {
        printf("Unable to start timer\n");
    }

    Z502Idle();
}
//****************************************************************************
void RemoveFromRunningListByID(long processID)
{
    ListNode* currentNode = RunningList->head;
    if( ((PCB*)currentNode->data)->processID == processID )
    {
        free(RunningList->head);
        RunningList->head = 0;
        currentNode = 0;
    }

    ListNode* prevNode = currentNode;
    currentNode = currentNode->next;
    while( currentNode )
    {
        if( ((PCB*)currentNode->data)->processID == processID )
        {
            prevNode->next = currentNode->next;
            free(currentNode);
            break;
        }
        currentNode = currentNode->next;
    }

    return;
}
//****************************************************************************
void RemoveFromTimerQueueByID(long processID)
{
    for( int i = 1; i <= TimerQueue->heap.size; ++i )
    {
        HeapItem* item = &(TimerQueue->heap.buffer[i]);
        if( ((PCB*)item->data)->processID == processID )
        {
            HeapItem temp;
            MinPriQueueRemove(TimerQueue, i, &temp);
            break;
        }
    }
}
//****************************************************************************
void RemoveFromReadyQueueByID(long processID)
{
    for( int i = 1; i <= ReadyQueue->heap.size; ++i )
    {
        HeapItem* item = &(ReadyQueue->heap.buffer[i]);
        if( ((PCB*)item->data)->processID == processID )
        {
            HeapItem temp;
            MinPriQueueRemove(ReadyQueue, i, &temp);
            break;
        }
    }
}
//****************************************************************************
void PopFromTimerQueue(PCB** ppcb)
{
    HeapItem item;
    MinPriQueuePop(TimerQueue, &item);
    *ppcb = (PCB*)item.data;
}
//****************************************************************************
void PushToTimerQueue(PCB* pcb)
{
    MinPriQueuePush(TimerQueue, pcb->timerQueueKey, pcb);
}
//****************************************************************************
void PopFromReadyQueue(PCB** ppcb)
{
    HeapItem item;
    MinPriQueuePop(ReadyQueue, &item);
    *ppcb = (PCB*)item.data;
}
//****************************************************************************
void PushToReadyQueue(PCB* pcb)
{
    MinPriQueuePush(ReadyQueue, pcb->readyQueueKey, pcb);
}
//****************************************************************************
void OSCreateProcess(char* name, ProcessEntry entry, int priority, long* dstID,
    long* dstErr)
{
    static long CurrentProcessID = 0;

    PCB* pcb = calloc(1, sizeof(PCB));
    pcb->entry = entry;
    pcb->priority = priority;
    pcb->readyQueueKey = priority;
    size_t len = strlen(name);
    pcb->name = malloc(len + 1);
    strcpy(pcb->name, name);
    pcb->name[len] = 0;
    pcb->processID = ++CurrentProcessID;

    // Return process id to the caller.
    if( dstID )
    {
        *dstID = pcb->processID;
    }
    if( dstErr )
    {
        *dstErr = ERR_SUCCESS;
    }

    // Add to global process list.
    ListNode* pcbNode = calloc(1, sizeof(ListNode));
    pcbNode->data = (void*)pcb;
    ListAttach(RunningList, pcbNode);

    // Add to ready queue.
    MinPriQueuePush(ReadyQueue, priority, pcb);
   
    Z502MakeContext(&pcb->context, (void*)entry, USER_MODE);
    Z502SwitchContext(SWITCH_CONTEXT_KILL_MODE, &pcb->context);
}
//****************************************************************************