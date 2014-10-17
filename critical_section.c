//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#include "critical_section.h"
#include "syscalls.h"
#include "protos.h"

#define DO_LOCK                     1
#define DO_UNLOCK                   0
#define SUSPEND_UNTIL_LOCKED        TRUE

int gFlags[2] = { 0, 0 };
int gTurn = 0;
INT32 gResult;

//****************************************************************************
void EnterCriticalSection(int i)
{
    gFlags[i] = 1;
    int j = (i + 1) % 2;
    gTurn = j;
    while( gFlags[j] == 1 && gTurn == j )
    {
        //printf("Waiting for cs.\n");
    }
    //READ_MODIFY(MEMORY_INTERLOCK_BASE, DO_LOCK, TRUE,
    //    &gResult);
}
//****************************************************************************
void LeaveCriticalSection(int i)
{
    gFlags[i] = 0;
    //READ_MODIFY(MEMORY_INTERLOCK_BASE, DO_UNLOCK, TRUE,
    //    &gResult);
}
//****************************************************************************