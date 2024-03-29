//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#ifndef SVC_H_
#define SVC_H_

#include "syscalls.h"

// SVC functions are called by SVC handler.
void SVCGetProcessID(SYSTEM_CALL_DATA* SystemCallData);
void SVCTerminateProcess(SYSTEM_CALL_DATA* SystemCallData);
void SVCCreateProcess(SYSTEM_CALL_DATA* SystemCallData);
void SVCStartTimer(SYSTEM_CALL_DATA* SystemCallData);
void SVCSuspendProcess(SYSTEM_CALL_DATA* SystemCallData);
void SVCResumeProcess(SYSTEM_CALL_DATA* SystemCallData);
void SVCChangeProcessPriority(SYSTEM_CALL_DATA* SystemCallData);
void SVCSendMessage(SYSTEM_CALL_DATA* SystemCallData);
void SVCReceiveMessage(SYSTEM_CALL_DATA* SystemCallData);
void SVCWriteDisk(SYSTEM_CALL_DATA* SystemCallData);
void SVCReadDisk(SYSTEM_CALL_DATA* SystemCallData);

#endif