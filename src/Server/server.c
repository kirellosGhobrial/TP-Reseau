#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server.h"
Client clients[MAX_CLIENTS];
int actual = 0;
Group groups[100];
int groupCount = 0;

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

static void app(void)
{
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   Request request;
   /* the index for the array */
   int max = sock;
   /* an array for all clients */

   fd_set rdfs;

   while(1)
   {
      int i = 0;
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the connection socket */
      FD_SET(sock, &rdfs);

      /* add socket of each client */
      for(i = 0; i < actual; i++)
      {
         FD_SET(clients[i].sock, &rdfs);
      }

      if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if(FD_ISSET(STDIN_FILENO, &rdfs))
      {
         /* stop process when type on keyboard */
         break;
      }
      else if(FD_ISSET(sock, &rdfs))
      {
         /* new client */
         SOCKADDR_IN csin = { 0 };
         size_t sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, (unsigned int *)&sinsize);
         if(csock == SOCKET_ERROR)
         {
            perror("accept()");
            continue;
         }

         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;

         FD_SET(csock, &rdfs);

         Client c = { csock };
         // strncpy(c.name, buffer, BUF_SIZE - 1);
         clients[actual] = c;
         actual++;
      }
      else
      {
         int i = 0;
         for(i = 0; i < actual; i++)
         {
            /* a client is talking */
            if(FD_ISSET(clients[i].sock, &rdfs))
            {
               Client* client = &(clients[i]);
               int c = read_client(clients[i].sock, &request);
               /* client disconnected */
               if(c == 0)
               {
                  closesocket(clients[i].sock);
                  remove_client( i);
               }
               else
               {
                  handle_request(client, &request);
               }
               break;
            }
         }
      }
   }

   clear_clients();
   end_connection(sock);
}

static void clear_clients()
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      closesocket(clients[i].sock);
   }
}

static void remove_client(int to_remove)
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (actual - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   actual--;
}

static Client* getClient(char * username){
   Client *cl = (Client *) malloc(sizeof(Client));
   int found =0;
   FILE * file;
   file = fopen("db/login.dat","rb");
   if(file != NULL){
      while(fread(cl, sizeof(Client), 1, file)){
         if(!strcmp(cl->name,username)){
            found = 1;
            break;
         }
      }
      fclose(file);
   } 
   
   if(found ==0){
      cl = NULL;
   }

   return cl;
}

static void saveClient(Client cl){
   Client * clTemp = getClient(cl.name);
   if(clTemp == NULL){
      FILE* file;
      file = fopen("db/login.dat", "ab+");
      fwrite(&cl, sizeof(Client), 1, file);
      fclose(file);
   }else{
      /* Update client if already exist */
      FILE * file;
      file = fopen("db/login.dat","rb+");
      if(file != NULL){
         while(fread(clTemp, sizeof(Client), 1, file)){
            if(!strcmp(clTemp->name,cl.name)){
               fseek(file, -sizeof(Client), SEEK_CUR);
               fwrite(&cl, sizeof(Client), 1, file);
               break;
            }
         }
         fclose(file);
      }
   }
   
}

static Group* getGroup(char * groupName){
   Group *group = (Group *) malloc(sizeof(Group));
   int found =0;
   FILE * file;
   file = fopen("db/groups.dat","rb");
   if(file != NULL){
      while(fread(group, sizeof(Group), 1, file)){
         if(!strcmp(group->name,groupName)){
            found = 1;
            break;
         }
      }
      fclose(file);
   } 
   
   if(found ==0){
      group = NULL;
   }
   return group;
}

static void saveGroup(Group group){
   Group * groupTemp = getGroup(group.name);
   if(groupTemp == NULL){
      FILE* file;
      file = fopen("db/groups.dat", "ab+");
      fwrite(&group, sizeof(Group), 1, file);
      fclose(file);
   }else{
      /* Update group if already exist*/
      FILE * file;
      file = fopen("db/groups.dat","rb+");
      if(file != NULL){
         while(fread(groupTemp, sizeof(Group), 1, file)){
            if(!strcmp(groupTemp->name,group.name)){
               fseek(file, -sizeof(Group), SEEK_CUR);
               fwrite(&group, sizeof(Group), 1, file);
               break;
            }
         }
         fclose(file);
      }
   }
}

static void handle_request(Client *sender, Request *req)
{
   switch(req->type)
   {
      case USER_LOGIN:
         handle_login( sender, req);
         break;
      case USER_REGISTER:
         handle_register(sender, req);
         break;
      case SEND_MESSAGE:
         handle_message( sender, req->message);
         break;
      case CREATE_GROUP:
         handle_create_group(sender, req);
         break;
      case JOIN_GROUP:
         handle_join_group(sender, req);
         break;
      case INVITE_USER:
         handle_invite_user(clients, sender, req, actual);
         break;
      case LIST_USERS:
         // handle_list_users(sender);
         break;
      default:
         break;
   }
}

static void handle_login(Client *client, Request *req)
{
   char name[BUF_SIZE];
   char password[BUF_SIZE];
   strncpy(name, req->params[0], BUF_SIZE - 1);
   strncpy(password, req->params[1], BUF_SIZE - 1);
   Response res;
   int i=0;
   for(int i=0; i<actual; i++){
      /* Check if user is already logged in */
      if(!strcmp(clients[i].name,name)){
         res.type = ERROR;
         res.paramCount = 1;
         strcpy(res.params[0], "Login failed");
         write_client(client->sock, &res);
         return;
      }
   }
   
   Client *clTemp = getClient(name);
   if(clTemp != NULL){
      if(!strcmp(clTemp->password, password)){
         res.type = OK;
         res.paramCount = 1;
         strcpy(res.params[0], "Login successful");
         strcpy(client->name, name);
         client->logged = 1;
      }else{
         res.type = ERROR;
         res.paramCount = 1;
         strcpy(res.params[0], "Login failed, incorrect password");
      }
   }else{
      res.type = ERROR;
      res.paramCount = 1;
      strcpy(res.params[0], "Login failed, username doesn't exist");
   }

   
   write_client(client->sock, &res);
   if(res.type == OK)  readUnreadMessages(client->name,10);

}

static void handle_register(Client *client, Request *req)
{
   char name[BUF_SIZE];
   char password[BUF_SIZE];
   strncpy(name, req->params[0], BUF_SIZE - 1);
   strncpy(password, req->params[1], BUF_SIZE - 1);

   Response res;

   Client * clTemp = getClient(name);
   if(clTemp == NULL){
      strcpy(client->name, name);
      strcpy(client->password, password);
      client->invitationCount = 0;
      client->logged = 1;

      saveClient(*client);
      strcpy(client->password, "");
      res.type = OK;
      res.paramCount = 1;
      strcpy(res.params[0], "Register successful");
   }else{
      res.type = ERROR;
      res.paramCount = 1;
      strcpy(res.params[0], "Username already exists");
   }
   write_client(client->sock, &res);
}

static void handle_create_group(Client *client, Request *req)
{
   char groupName[BUF_SIZE];
   strncpy(groupName, req->params[0], BUF_SIZE - 1);
   Response res;
   Group *group = getGroup(groupName);
   if (group == NULL)
   {
      group = (Group *)malloc(sizeof(Group));
      strcpy(group->name, groupName);
      strcpy(group->members[0], client->name);
      group->memberCount = 1;
      saveGroup(*group);
      res.type = OK;
      strcpy(res.params[0], "Group created successfully");
   }
   else
   {
      res.type = ERROR;
      strcpy(res.params[0], "Group already exists");
   }
   res.paramCount = 1;
   write_client(client->sock, &res);
}

static void handle_invite_user(Client *clients, Client *sender, Request *req, int actual)
{
   char groupName[20];
   char username[20];
   strncpy(groupName, req->params[0], 20);
   strncpy(username, req->params[1], 20);
   Response res;
   Group *group = getGroup(groupName);
   if (group == NULL)
   {
      res.type = ERROR;
      res.paramCount = 1;
      strcpy(res.params[0], "Group doesn't exist");
      write_client(sender->sock, &res);
      return;
   }

   for (int j = 0; j < group->memberCount; j++)
   {
      if (!strcmp(group->members[j], sender->name))
      {
         // Check if user exists in db
         Client *client = getClient(username);
         if (client == NULL)
         {
            res.type = ERROR;
            res.paramCount = 1;
            strcpy(res.params[0], "User doesn't exist");
            write_client(sender->sock, &res);
            return;
         }
         else
         {
            // Check if the user is already in the group
            for (int k = 0; k < group->memberCount; k++)
            {
               if (!strcmp(group->members[k], client->name))
               {
                  res.type = ERROR;
                  res.paramCount = 1;
                  strcpy(res.params[0], "User already in group");
                  write_client(sender->sock, &res);
                  return;
               }
            }
            strcpy(client->invitations[client->invitationCount], group->name);
            client->invitationCount++;
            saveClient(*client);
            res.type = OK;
            res.paramCount = 1;
            strcpy(res.params[0], "User invited successfully");
            write_client(sender->sock, &res);

            // Send invitation to the user if he is online
            for (int i = 0; i < actual; i++)
            {
               if (!strcmp(clients[i].name, client->name))
               {
                  res.type = OK;
                  res.paramCount = 1;
                  char message[BUF_SIZE];
                  sprintf(message, "You have been invited to group %s by %s", groupName, sender->name);
                  strcpy(res.params[0], message);
                  write_client(clients[i].sock, &res);
                  return;
               }
            }   
            return;         
         }
      }
   }
   res.type = ERROR;
   res.paramCount = 1;
   strcpy(res.params[0], "You are not a member of this group");
   write_client(sender->sock, &res);
   return;
}

static void handle_join_group(Client *sender, Request *req)
{
   char groupName[BUF_SIZE];
   strncpy(groupName, req->params[0], BUF_SIZE - 1);
   Response res;
   Group *group = getGroup(groupName);
   if (group == NULL)
   {
      res.type = ERROR;
      res.paramCount = 1;
      strcpy(res.params[0], "Group doesn't exist");
      write_client(sender->sock, &res);
      return;
   }
   for (int i = 0; i < group->memberCount; i++)
   {
      if (!strcmp(group->members[i], sender->name))
      {
         res.type = ERROR;
         res.paramCount = 1;
         strcpy(res.params[0], "You are already a member of this group");
         write_client(sender->sock, &res);
         return;
      }
   }
   Client *client = getClient(sender->name);
   int i = 0;
   for (i = 0; i < client->invitationCount; i++)
   {
      if (!strcmp(client->invitations[i], groupName))
      {
         strcpy(group->members[group->memberCount], client->name);
         group->memberCount++;
         saveGroup(*group);
         strcpy(client->invitations[i], client->invitations[client->invitationCount - 1]);
         client->invitationCount--;
         saveClient(*client);
         res.type = OK;
         res.paramCount = 1;
         strcpy(res.params[0], "You have joined the group successfully");
         write_client(sender->sock, &res);
         return;
      }
   }
   res.type = ERROR;
   res.paramCount = 1;
   strcpy(res.params[0], "You have not been invited to this group");
   write_client(sender->sock, &res);
   return;
}

static void handle_message(Client *sender, Message msg)
{
   Response res;
   res.type = MESSAGE;
   res.paramCount = 0;
   strcpy(res.message.content, msg.content);
   strcpy(res.message.receiver, msg.receiver);
   strcpy(res.message.sender, sender->name);
   res.message.type = msg.type;

   switch(msg.type)
   {
      case PUBLIC_MESSAGE:
         send_public_message( &res);
         break;
      case PRIVATE_MESSAGE:
         send_private_message( &res);
         break;
      case GROUP_MESSAGE:
         send_group_message( &res);
         break;
      default:
         break;
   }
}

static void send_public_message(Response *res)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if(strcmp (clients[i].name, res->message.sender) != 0 && clients[i].logged==1 )
      {
         write_client(clients[i].sock, res);
      }
   }
}

static void send_private_message(Client *sender, Response *res)
{
   Client *client = getClient(res->message.receiver);
   if (client == NULL)
   {
      res->type = ERROR;
      res->paramCount = 1;
      strcpy(res->params[0], "User doesn't exist");
      write_client(sender->sock, res);
      return;
   }
   int i = 0;
   for(i=0; i<actual; i++)
   {
      if (strcmp (clients[i].name, res->message.receiver) == 0)
      {
         write_client (clients[i].sock, res);
         return;
      }
   }
   if(getClient(res->message.receiver)){
      addUnreadMessage(res->message.receiver, res->message);
   }

static void send_group_message(Client *clients, Client *sender, Response *res, int actual)
{
   Group *group = getGroup(res->message.receiver);
   if (group == NULL)
   {
      res->type = ERROR;
      res->paramCount = 1;
      strcpy(res->params[0], "Group doesn't exist");
      write_client(sender->sock, res);
      return;
   }
   /* Check if the sender is a member of the group */
   int i = 0;
   for (i = 0; i < group->memberCount; i++)
   {
      if (!strcmp(group->members[i], sender->name))
      {
         break;
      }
   }
   if (i == group->memberCount)
   {
      res->type = ERROR;
      res->paramCount = 1;
      strcpy(res->params[0], "You are not a member of this group");
      write_client(sender->sock, res);
      return;
   }
   for (i = 0; i < group->memberCount; i++)
   {
      printf("Member: %s", group->members[i]);
      for (int j = 0; j < actual; j++)
      {
         if (!strcmp(clients[j].name, group->members[i]))
         {
            printf("Sending message to %s\n", clients[j].name);
            write_client(clients[j].sock, res);
         }
      }
   }
}

static void addUnreadMessage(char* username, Message msg){
   if(getClient(username)){
      FILE * file;
      char dest[BUF_SIZE];
      strcpy(dest,"db/users/");
      strcat(dest,username);
      file = fopen(dest,"ab+");
      fwrite(&msg,sizeof(Message),1,file);
      fclose(file);
   }
}

static void readUnreadMessages(char* username, int nbMsg){
   Client * clSender;
   Client* clTemp = getClient(username);
   if(clTemp != NULL){
      FILE * file;
      char dest[BUF_SIZE];
      strcpy(dest,"db/users/");
      strcat(dest,username);
      file = fopen(dest,"ab+");
      Message msg;
      while(fread(&msg,sizeof(Message),1,file) && nbMsg>0){
         clSender = getClient(msg.sender);
         if(clSender != NULL)  handle_message(clSender,msg);
         nbMsg--;
      }
      freopen(dest,"w", file);
      fclose(file);
   }
}

static int init_connection(void)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   int opt =1;
   if(setsockopt(sock,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
      perror("setsockopt");
      exit(EXIT_FAILURE);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
      exit(errno);
   }

   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_client(SOCKET sock, Request *req)
{
   int n = 0;

   if((n = recv(sock, req, sizeof(*req), 0)) < 0)
   {
      perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   return n;
}

static void write_client(SOCKET sock, Response *res)
{
   if(send(sock, res, sizeof(*res), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }

}

int main(int argc, char **argv)
{
   init();

   app();

   end();

   return EXIT_SUCCESS;
}
