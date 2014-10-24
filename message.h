//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "os_common.h"

// Process message.
typedef struct _Message
{
    long senderProcessID;
    long length;
    char buffer[LEGAL_MESSAGE_LENGTH_MAX];
} Message;

#endif