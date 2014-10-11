#ifndef SVC_H_
#define SVC_H_

#include "syscalls.h"

void SVCGetProcessID(SYSTEM_CALL_DATA* SystemCallData);
void SVCTerminateProcess(SYSTEM_CALL_DATA* SystemCallData);
void SVCCreateProcess(SYSTEM_CALL_DATA* SystemCallData);
void SVCStartTimer(SYSTEM_CALL_DATA* SystemCallData);
void SVCSuspendProcess(SYSTEM_CALL_DATA* SystemCallData);
void SVCResumeProcess(SYSTEM_CALL_DATA* SystemCallData);
void SVCChangeProcessPriority(SYSTEM_CALL_DATA* SystemCallData);

#endif