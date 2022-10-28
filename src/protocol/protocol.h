#ifndef PROTOCOL_H
#define PROTOCOL_H

#define BUF_SIZE    1024
#define PARAM_NUMBER 10

typedef enum
{
  PUBLIC_MESSAGE,
  PRIVATE_MESSAGE,
  GROUP_MESSAGE,
} message_type;

typedef struct
{
  message_type type;
  char sender[BUF_SIZE];
  char receiver[BUF_SIZE];
  char content[BUF_SIZE];
  int groupID;
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