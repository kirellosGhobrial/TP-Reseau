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

typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
   char password[BUF_SIZE];
   int logged;
}Client;

static void init(void);
static void end(void);
static void app(void);
static int init_connection(void);
static void end_connection(int sock);
static int read_client(SOCKET sock, Request *req);
static void write_client(SOCKET sock, Response *res);
static void remove_client(Client *clients, int to_remove, int *actual);
static void clear_clients(Client *clients, int actual);

static void handle_request(Client *clients, Client *sender, Request *req, int actual);
static void handle_login(Client* clients, int actual, Client *sender, Request *req);
static void handle_register(Client *sender, Request *req);


static void handle_message(Client *clients, Client *sender, Message *msg, int actual);
static void send_public_message(Client *clients, Response *res, int actual);
static void send_private_message(Client *clients, Response *res, int actual);
static void send_group_message(Client *clients, Response *res, int actual);

static Client* getClient(char * username);
static int saveClient(Client cl);


#endif /* guard */
