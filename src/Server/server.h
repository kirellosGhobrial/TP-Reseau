#ifndef SERVER_H
#define SERVER_H

#ifdef WIN32

#include <winsock2.h>

#elif defined (linux)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#include "../protocol/protocol.h"
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else

#error not defined for this platform

#endif

#define CRLF        "\r\n"
#define PORT         1977
#define MAX_CLIENTS     100
#define BUF_SIZE    1024
#define MAX_INVITATIONS 10
#define MAX_GROUPS 100

typedef struct
{
   SOCKET sock;
   char name[20];
   char password[20];
   int logged;
   char invitations[MAX_INVITATIONS][20];
   int invitationCount;
} Client;

static void init(void);
static void end(void);
static void app(void);
static int init_connection(void);
static void end_connection(int sock);
static int read_client(SOCKET sock, Request *req);
static void write_client(SOCKET sock, Response *res);
static void remove_client( int to_remove);
static void clear_clients();

static void handle_request(Client *sender, Request *req);
static void handle_login( Client *sender, Request *req);
static void handle_register(Client *sender, Request *req);

static void handle_create_group(Client *sender, Request *req);
static void handle_join_group(Client *client, Request *req);
static void handle_invite_user(Client *sender, Request *req);

static void handle_message( Client *sender, Message msg);
static void send_public_message( Response *res);
static void send_private_message( Client *sender, Response *res);
static void send_group_message( Client *sender, Response *res);

static Client* getClient(char * username);
static void saveClient(Client cl);

static Group* getGroup(char * groupName);
static void saveGroup(Group group);

static void handle_list_users(Client *sender);

static void addUnreadMessage(char* username, Message msg);
static void readUnreadMessages(char* username);
static void readUnreadNotifications(char* username);
static void addUnreadNotification(char* username, Response res);
static void logRequest(Client* cl, Request* req);
static void logResponse(Client* cl, Response* res);

static Client* findClient(SOCKET sk);


#endif /* guard */
