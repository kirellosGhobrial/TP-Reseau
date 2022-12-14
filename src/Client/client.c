#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "client.h"

static void init(void)
{
#ifdef WIN32
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if(err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }
#endif
}

static void end(void)
{
#ifdef WIN32
   WSACleanup();
#endif
}

static void app(const char *address)
{
   SOCKET sock = init_connection(address);
   char buffer[BUF_SIZE];
   Request request;
   Response response;

   fd_set rdfs;

   while(1)
   {
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the socket */
      FD_SET(sock, &rdfs);

      if(select(sock + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if(FD_ISSET(STDIN_FILENO, &rdfs))
      {
         handle_user_input(sock, &request);
      }
      else if(FD_ISSET(sock, &rdfs))
      {
         handle_server_response(sock, &response);
      }
   }

   end_connection(sock);
}

static int init_connection(const char *address)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };
   struct hostent *hostinfo;

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   hostinfo = gethostbyname(address);
   if (hostinfo == NULL)
   {
      fprintf (stderr, "Unknown host %s.\n", address);
      exit(EXIT_FAILURE);
   }

   sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr;
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
   {
      perror("connect()");
      exit(errno);
   }

   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_server(SOCKET sock, Response *res)
{  
   int n = 0;

   if((n = recv(sock, res, sizeof(*res), 0)) < 0)
   {
      perror("recv()");
      exit(errno);
   }

   return n;
}

static void write_server(SOCKET sock, Request *req)
{
   if(send(sock, req, sizeof(*req), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

static void handle_user_input(SOCKET sock, Request *req)
{
   char buffer[BUF_SIZE];
   fgets(buffer, BUF_SIZE - 1, stdin);
   int len = strlen (buffer) - 1;
   buffer[len] = '\0';

   if (strncmp(buffer, "/quit", 5) == 0)
   {
      printf("Bye !\n");
      end_connection(sock);
      exit(0);
   }
   else if (strncmp(buffer, "/help", 5) == 0)
   {
      printf("Commands:\n");
      printf("/help: display this help\n");
      printf("/login: Login to your account\n");
      printf("\t Usage: /login <username> <password>\n");
      printf("/register: Create a new account\n");
      printf("\t Usage: /register <username> <password>\n");
      printf("/private : Send a private message to a user\n");
      printf("\t Usage: /private <username> <message>\n");
      printf("/public : Send a public message to all users\n");
      printf("\t Usage: /public <message>\n");
      printf("/group : Send a message to a group\n");
      printf("\t Usage: /group <groupname> <message>\n");
      printf("/create : Create a new group\n");
      printf("\t Usage: /create <groupname>\n");
      printf("/join : Join a group\n");
      printf("\t Usage: /join <groupname>\n");
      printf("/invite : Invite a user to a group\n");
      printf("\t Usage: /invite <groupname> <username>\n");
      printf("/list_users: list all connected users\n");
      printf("/quit: quit the program\n");
   }
   else if (strncmp(buffer, "/register", 9) == 0)
   {
      char *name = strtok(buffer + 9, " ");
      char *password = strtok(NULL, "");
      if (name == NULL || password == NULL)
      {
         printf("Usage : /register <name> <password>\n");
      }
      else
      {
         req->type = USER_REGISTER;
         req->paramCount = 2;
         strcpy(req->params[0], name);
         strcpy(req->params[1], password);
         write_server(sock, req);
      }
   }
   else if (strncmp(buffer, "/login", 6) == 0)
   {
      char *name = strtok(buffer + 6, " ");
      char *password = strtok(NULL, "");
      if (name == NULL || password == NULL)
      {
         printf("Usage : /login <name> <password>\n");
      }
      else
      {
         req->type = USER_LOGIN;
         req->paramCount = 2;
         strcpy(req->params[0], name);
         strcpy(req->params[1], password);
         write_server(sock, req);
      }
   }
   else if (strncmp(buffer, "/private", 8) == 0)
   {
      char *name = strtok(buffer + 8, " ");
      char *input_msg = strtok(NULL, "");
      if (name == NULL || input_msg == NULL)
      {
         printf("Usage : /private <name> <message>\n");
      }
      else
      {   
         req->message.type = PRIVATE_MESSAGE;
         strcpy(req->message.receiver, name);
         strcpy(req->message.content, input_msg);
         req->type = SEND_MESSAGE;
         write_server(sock, req);
      }
   }
   else if (strncmp(buffer, "/public", 7) == 0)
   {
      char *input_msg = strtok(buffer + 7, "");
      if (input_msg == NULL)
      {
         printf("Usage : /public <message>\n");
      }
      else
      {
         req->message.type = PUBLIC_MESSAGE;
         strcpy(req->message.content, input_msg);
         req->type = SEND_MESSAGE;
         write_server(sock, req);
      }
   }
   else if (strncmp(buffer, "/group", 6) == 0)
   {
      char *name = strtok(buffer + 6, " ");
      char *input_msg = strtok(NULL, "");
      if (name == NULL || input_msg == NULL)
      {
         printf("Usage : /group <groupname> <message>\n");
      }
      else
      {
         req->message.type = GROUP_MESSAGE;
         strcpy(req->message.receiver, name);
         strcpy(req->message.content, input_msg);
         req->type = SEND_MESSAGE;
         write_server(sock, req);
      }
   }
   else if (strncmp(buffer, "/create", 7) == 0)
   {
      char *name = strtok(buffer + 7, " ");
      if (name == NULL)
      {
         printf("Usage : /create <GroupName>\n");
      }
      else
      {
         req->type = CREATE_GROUP;
         req->paramCount = 1;
         strcpy(req->params[0], name);
         write_server(sock, req);
      }
   }
   else if (strncmp(buffer, "/invite", 7) == 0)
   {
      char *group_name = strtok(buffer + 7, " ");
      char *user_name = strtok(NULL, "");
      if (group_name == NULL || user_name == NULL)
      {
         printf("Usage : /invite <groupname> <username>\n");
      }
      else
      {
         req->type = INVITE_USER;
         req->paramCount = 2;
         strcpy(req->params[0], group_name);
         strcpy(req->params[1], user_name);
         write_server(sock, req);
      }
   }
   else if (strncmp(buffer, "/join", 5) == 0)
   {
      char *name = strtok(buffer + 5, " ");
      if (name == NULL)
      {
         printf("Usage : /join <groupname>\n");
      }
      else
      {
         req->type = JOIN_GROUP;
         req->paramCount = 1;
         strcpy(req->params[0], name);
         write_server(sock, req);
      }
   }
   else if (strncmp(buffer, "/list_users", 11) == 0)
   {
      req->type = LIST_USERS;
      write_server(sock, req);
   }
   else
   {
      printf("Invalid command. Type /help for a list of commands.\n");
   }
}

static void handle_server_response(SOCKET sock, Response *res)
{
   int n = read_server(sock, res);
   /* server down */
   if(n == 0)
   {
      printf("Server disconnected !\n");
      exit(0);
   }

   switch (res->type)
   {
   case OK:
      for (int i = 0; i < res->paramCount; i++)
      {
         printf(GRN "%s" RESET "\n", res->params[i]);
      }
      break;
   case MESSAGE: 
      if (res->message.type == PUBLIC_MESSAGE) printf( BLU BOLD "[%s on public]: " RESET "%s\n", res->message.sender, res->message.content);
      else if(res->message.type == PRIVATE_MESSAGE) printf( BLU BOLD "[%s on private]: " RESET "%s\n", res->message.sender, res->message.content);
      else if(res->message.type == GROUP_MESSAGE) printf( BLU BOLD "[%s on %s]: " RESET "%s\n", res->message.sender, res->message.receiver, res->message.content);
      break;
   case ERROR:
      for (int i = 0; i < res->paramCount; i++)
      {
         printf(RED "Error : %s" RESET "\n", res->params[i]);
      }
      break;
   default:
      break;
   }
}

int main(int argc, char **argv)
{
   if (argv[1] == NULL)
   {
      printf("Usage : %s <address>\n", argv[0]);
      return 1;
   }

   init();

   app(argv[1]);

   end();

   return EXIT_SUCCESS;
}
