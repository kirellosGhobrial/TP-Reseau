#ifndef CLIENT_H
#define CLIENT_H

#include "server2.h"

typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
   int chatId;
   char pwd[BUF_SIZE];
   int connected;

}Client;

#endif /* guard */
