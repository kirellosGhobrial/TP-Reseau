#ifndef CLIENT_H
#define CLIENT_H

#include "server.h"

typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
   char password[BUF_SIZE];
   int logged;
}Client;

typedef struct
{
  int groupID;
  char groupName[BUF_SIZE];
  char owner[BUF_SIZE];
  char *members[BUF_SIZE];
} Groupe;

#endif /* guard */
