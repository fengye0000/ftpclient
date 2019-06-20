#ifndef COMMON_H
#define COMMON_H

#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>		// getaddrinfo()
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>



/* constants */
#define DEBUG				1
#define MAXSIZE 			512 	// max buffer size
#define CLIENT_PORT_ID		30020


/* Holds command code and argument */
struct command {
	char arg[255];
	char code[5];
   
    
};
struct req{
    int code;
    char arg[400];
};



/**
 * Create listening socket on remote host
 * Returns -1 on error, socket fd on success
 */
int socket_create(int port);


/**
 * Create new socket for incoming client connection request
 * Returns -1 on error, or fd of newly created socket
 */
int socket_accept(int sock_listen);


/**
 * Connect to remote host at given port
 * Returns socket fd on success, -1 on error
 */
int socket_connect(int port, char *host);



/**
 * Receive data on sockfd
 * Returns -1 on error, number of bytes received 
 * on success
 */
int recv_data(int sockfd, char* buf, int bufsize);


/**
 * Send resposne code on sockfd
 * Returns -1 on error, 0 on success
 */
int send_response(int sockfd, int rc);



//------------------- UTILITY FUNCTIONS-------------------//

/**
 * Trim whiteshpace and line ending
 * characters from a string
 */
void trimstr(char *str, int n);



/** 
 * Read input from command line
 */
void read_input(char* buffer, int size);


#include <dirent.h>

#define    DEBUG        1

#define    PORTSERVER    8487
#define CONTROLPORT    PORTSERVER
#define DATAPORT    (PORTSERVER + 1)

enum TYPE
{
    REQU,
    DONE,
    INFO,
    TERM,
    DATA,
    EOT
};

#define    NP        0
#define    HP        1

#define    er(e, x)                    \
do                        \
{                        \
perror("ERROR IN: " #e "\n");        \
fprintf(stderr, "%d\n", x);        \
exit(-1);                \
}                        \
while(0)

#define    LENBUFFER    504        // so as to make the whole packet well-rounded ( = 512 bytes)
struct packet
{
    short int conid;
    short int type;
    short int comid;
    short int datalen;
    char buffer[LENBUFFER];
};

void set0(struct packet*);

struct packet* ntohp(struct packet*);
struct packet* htonp(struct packet*);

void printpacket(struct packet*, int);

#define NCOMMANDS 19
enum COMMAND
{
    GET,
    PUT,
    MGET,
    MPUT,
    CD,
    LCD,
    MGETWILD,
    MPUTWILD,
    DIR_,        // _ to avoid conflict with directory pointer DIR
    LDIR,
    LS,
    LLS,
    MKDIR,
    LMKDIR,
    RGET,
    RPUT,
    PWD,
    LPWD,
    EXIT
};            // any change made here should also be \
replicated accordingly in the commandlist \
2D array in client_ftp_fucntions.c




#endif







