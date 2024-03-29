//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

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

#include "os_common.h"
#include "svc.h"
#include "process_manager.h"
#include "scheduler.h"
#include "memory_manager.h"
#include "interrupt_handler.h"
#include "fault_handler.h"
#include "critical_section.h"
#include "disk_manager.h"

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
        IHTimerInterrupt();
        break;

    case 5:  // disk 1
    case 6:  // disk 2
    case 7:  // disk 3
    case 8:  // disk 4
    case 9:  // disk 5
    case 10: // disk 6
    case 11: // disk 7
    case 12: // disk 8
        IHDiskInterrupt(device_id);
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

    //printf( "Fault_handler: Found vector type %d with value %d\n",
    //                    device_id, status );

    switch( device_id )
    {
    case 2:
        FHMemoryFault(status);
        break;

    case 4:
        Z502Halt();
        break;

    default:
        break;
    }

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

    case SYSNUM_SUSPEND_PROCESS:
        SVCSuspendProcess(SystemCallData);
        break;

    case SYSNUM_RESUME_PROCESS:
        SVCResumeProcess(SystemCallData);
        break;

    case SYSNUM_CHANGE_PRIORITY:
        SVCChangeProcessPriority(SystemCallData);
        break;

    case SYSNUM_SEND_MESSAGE:
        SVCSendMessage(SystemCallData);
        break;

    case SYSNUM_RECEIVE_MESSAGE:
        SVCReceiveMessage(SystemCallData);
        break;

    case SYSNUM_DISK_WRITE:
        SVCWriteDisk(SystemCallData);
        break;

    case SYSNUM_DISK_READ:
        SVCReadDisk(SystemCallData);
        break;

    default:
        printf("ERROR!  call_type not recognized!\n");
        printf("Call_type is - %i\n", call_type);
        break;
    }
}                                               // End of svc

// Test entry functions.
void mytest(void);
ProcessEntry tests[20] = { test1a, 
                           test1b,
                           test1c, 
                           test1d, 
                           test1e, 
                           test1f, 
                           test1g, 
                           test1h, 
                           test1i, 
                           test1j,
                           test1k,
                           mytest,
                           test2a,
                           test2b,
                           test2c,
                           test2d,
                           test2e,
                           test2f,
                           test2g,
                           test2h };

/************************************************************************
    osInit
        This is the first routine called after the simulation begins.  This
        is equivalent to boot code.  All the initial OS components can be
        defined and initialized here.
************************************************************************/

void    osInit( int argc, char *argv[]  ) {
    void                *next_context;
    INT32               i;

    if( argc == 1 )
    {
        printf("Please enter a test you want to run (such as test1c).\n");
        return;
    }

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

    // Figure out which test we need to run.
    int entry = 0;
    char* entryName = "";
    if( argc > 1 )
    {
        if( argv[1][0] == 't' &&
            argv[1][1] == 'e' &&
            argv[1][2] == 's' &&
            argv[1][3] == 't' )
        {
            if( argv[1][4] == '1' || argv[1][4] == '2' )
            {
                entry = argv[1][5] - 'a';
                entryName = &argv[1][0];
                if( entry > 11 )
                {
                    entry = 11;
                }

                if( argv[1][4] == '2' )
                {
                    entry += 12;
                }
            }
        }
    }

    // Initialize global managers.
    ProcessManagerInitialize();
    SchedulerInitialize();
    MemoryManagerInitialize();
    DiskManagerInitialize();

    // Create main user process.
    PCB* pcb = gProcessManager->CreateProcess(entryName, 1, tests[entry], 20, 0, 0);
    gScheduler->Dispatch();
}                                               // End of osInit