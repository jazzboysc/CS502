#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "os_common.h"

typedef struct _Message
{
    long senderProcessID;
    long length;
    char buffer[LEGAL_MESSAGE_LENGTH_MAX];
} Message;

#endif