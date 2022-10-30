#ifndef PROTOCOL_H
#define PROTOCOL_H

#define BUF_SIZE    1024
#define PARAM_NUMBER 10
#define NAME_SIZE   32
#define MAX_MEMBERS 50

typedef struct
{
  char name[NAME_SIZE];
  int memberCount;
  char members[MAX_MEMBERS][NAME_SIZE];
} Group;

typedef enum
{
  PUBLIC_MESSAGE,
  PRIVATE_MESSAGE,
  GROUP_MESSAGE,
} message_type;

typedef struct
{
  message_type type;
  char sender[20];
  char receiver[20];
  char content[BUF_SIZE];
  int group;
} Message;

typedef enum
{
  USER_LOGIN,
  USER_REGISTER,
  CREATE_GROUP,
  JOIN_GROUP,
  LEAVE_GROUP,
  INVITE_USER,
  SEND_MESSAGE,
  LIST_USERS
} request_type;

typedef struct
{
  request_type type;
  int paramCount;
  char params[PARAM_NUMBER][BUF_SIZE];
  Message message;
} Request;

typedef enum
{
  OK,
  ERROR,
  MESSAGE,
  LIST
} response_type;

typedef struct
{
  response_type type;
  int paramCount;
  char params[PARAM_NUMBER][BUF_SIZE];
  Message message;
} Response;

#endif /* guard */