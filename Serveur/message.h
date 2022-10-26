#ifndef MESSAGE_H
#define MESSAGE_H

#include "server2.h"

typedef struct
{
   Client sender;
   char message[BUF_SIZE];
    
}Message;

#endif /* guard */
