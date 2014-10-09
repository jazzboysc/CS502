#include "critical_section.h"

int gFlags[2] = { 0, 0 };
int gTurn = 0;

//****************************************************************************
void EnterCriticalSection(int i)
{
    gFlags[i] = 1;
    int j = (i + 1) % 2;
    gTurn = j;
    while( gFlags[j] == 1 && gTurn == j )
    {
        if( i == 0 )
        {
            //printf("User process is waiting for entering critical section.\n");
        }
        else
        {
            //printf("Interrupt handler is waiting for entering critical section.\n");
        }
    }
}
//****************************************************************************
void LeaveCriticalSection(int i)
{
    gFlags[i] = 0;
}
//****************************************************************************