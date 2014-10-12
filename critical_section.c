#include "critical_section.h"

int gFlags[2] = { 0, 0 };
int gTurn = 0;

//****************************************************************************
void EnterCriticalSection(int i)
{
    gFlags[i] = 1;
    int j = (i + 1) % 2;
    gTurn = j;
    while( gFlags[j] == 1 && gTurn == j );
}
//****************************************************************************
void LeaveCriticalSection(int i)
{
    gFlags[i] = 0;
}
//****************************************************************************