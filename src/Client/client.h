#ifndef CLIENT_H
#define CLIENT_H

#ifdef WIN32

#include <winsock2.h>

#elif defined (linux)

#include "../protocol/protocol.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
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

#define CRLF     "\r\n"
#define PORT     1977

#define BUF_SIZE 1024

#define RED  "\x1B[31m"
#define GRN  "\x1B[32m"
#define YEL  "\x1B[33m"
#define BLU  "\x1B[34m"
#define BOLD "\033[1m"
#define RESET "\033[0m"

static void init(void);
static void end(void);
static void app(const char *address);
static int init_connection(const char *address);
static void end_connection(int sock);
static int read_server(SOCKET sock, Response *res);
static void write_server(SOCKET sock, Request *req);
static void handle_user_input(SOCKET sock, Request *req);
static void handle_server_response(SOCKET sock, Response *res);

#endif /* guard */
