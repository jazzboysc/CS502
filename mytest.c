//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#define          USER
#include         "global.h"
#include         "protos.h"
#include         "syscalls.h"

#include         "stdio.h"
#include         "string.h"
#include         "stdlib.h"
#include         "math.h"

INT16 Z502_PROGRAM_COUNTER;

extern long Z502_REG1;
extern long Z502_REG2;
extern long Z502_REG3;
extern long Z502_REG4;
extern long Z502_REG5;
extern long Z502_REG6;
extern long Z502_REG7;
extern long Z502_REG8;
extern long Z502_REG9;
extern INT16 Z502_MODE;

#define PRIORITY_1 1
#define PRIORITY_2 10
#define PRIORITY_3 50
#define PRIORITY_4 100
#define PRIORITY_5 180

//****************************************************************************
void mytestx(void)
{
    int    i;

    GET_PROCESS_ID("", &Z502_REG2, &Z502_REG9);
    printf("Release %s: mytestx: Pid %ld\n", CURRENT_REL, Z502_REG2);

    for( i = 0; i < 50; i++ )
    {
        printf("PID: %d is printing.\n", Z502_REG2);
        SLEEP(2);
    }

    TERMINATE_PROCESS(-1, &Z502_REG9);
    printf("ERROR: mytestx should be terminated but isn't.\n");

}
//****************************************************************************
// Preemptive-based Dispatching Test
// Please type “cs502 test1l?and see my preemptive algorithm implemented in 
// the scheduler module. Notice that processes 3 and 4 with high priorities 
// get much more chances running than process 5, 6 and 7 who have low 
// priorities.
//****************************************************************************
void mytest(void)
{
    CREATE_PROCESS("mytest_a", mytestx, PRIORITY_1, &Z502_REG3, &Z502_REG9);
    CREATE_PROCESS("mytest_b", mytestx, PRIORITY_2, &Z502_REG4, &Z502_REG9);
    CREATE_PROCESS("mytest_c", mytestx, PRIORITY_3, &Z502_REG5, &Z502_REG9);
    CREATE_PROCESS("mytest_d", mytestx, PRIORITY_4, &Z502_REG6, &Z502_REG9);
    CREATE_PROCESS("mytest_e", mytestx, PRIORITY_5, &Z502_REG7, &Z502_REG9);

    SLEEP(2000);

    TERMINATE_PROCESS(-2, &Z502_REG9);
}
//****************************************************************************