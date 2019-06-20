#include "ftclient.h"
#include<termios.h>
#include <unistd.h>
#include<regex.h>
#include<limits.h>

#include <time.h>
int sock_control;
char Remotehost[20];
#define USERNAME    220
#define PASSWORD    331
#define LOGIN        230
#define PATHNAME    257
#define CLOSEDATA    226
#define ACTIONOK    250
#define BUFFER_SIZE 255
#define sleep_time 500//ms
/* define macros*/
#define NONE "\e[0m"
#define RED  "\e[0;31m"
#define BLUE "\e[0;34m"
#define MAXBUF    1024
#define STDIN_FILENO    0
#define STDOUT_FILENO    1
#define Filelocate ""
//#define Filelocate "/Users/fengye/Desktop/ftpcli/file/"
//char Host[20]="172.20.10.3";
//char Host[20]="10.128.204.171";
//char Host[20]="192.168.5.21";
char transmit[10]="off";
int  transmit_rate=100000;//bytes per second
char Host[20]="10.128.204.104";
int data_sock;
int active_sock;
char    *rbuf, *rbuf1;
char    mode[10]="passive";
int     count=5;
int     ccount;
int     Active_port=13363;


 int socket_create(int port)
 {
 int sockfd;
 int yes = 1;
 struct sockaddr_in sock_addr;
 
 // create new socket
 if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
 perror("socket() error");
 return -1;
 }
 
 // set local address info
 sock_addr.sin_family = AF_INET;
 sock_addr.sin_port = htons(port);
 sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
 
 if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
 close(sockfd);
 perror("setsockopt() error");
 return -1;
 }
 
 // bind
 if (bind(sockfd, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) < 0) {
 close(sockfd);
 perror("bind() error");
 return -1;
 }
 
 // begin listening for incoming TCP requests
 if (listen(sockfd, 5) < 0) {
 close(sockfd);
 perror("listen() error");
 return -1;
 }
 
 return sockfd;
 }
 
 
 
 
 
 /**
 * Create new socket for incoming client connection request
 * Returns -1 on error, or fd of newly created socket
 */
int socket_accept(int sock_listen)
{
    int sockfd;
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    
    // Wait for incoming request, store client info in client_addr
    sockfd = accept(sock_listen, (struct sockaddr *) &client_addr, &len);
    
    if (sockfd < 0) {
        perror("accept() error");
        return -1;
    }
    return sockfd;
}




/**
 * Connect to remote host at given port
 * Returns:    socket fd on success, -1 on error
 */
int socket_connect(int port, char*host)
{
    int sockfd;
    struct sockaddr_in dest_addr;
    
    // create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("error creating socket");
        return -1;
    }
    
    // create server address
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr.s_addr = inet_addr(host);
    
    // Connect on socket
    if(connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0 ) {
        perror("error connecting to server");
        return -1;
    }
    return sockfd;
}



/**
 * Receive data on sockfd
 * Returns -1 on error, number of bytes received
 * on success
 */




/**
 * Trim whiteshpace and line ending
 * characters from a string
 */
void trimstr(char *str, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        if (isspace(str[i])) str[i] = 0;
        if (str[i] == '\n') str[i] = 0;
    }
}


/**
 * Send resposne code on sockfd
 * Returns -1 on error, 0 on success
 */
int send_response(int sockfd, int rc)
{
    int conv = htonl(rc);
    if (send(sockfd, &conv, sizeof conv, 0) < 0 ) {
        perror("error sending...\n");
        return -1;
    }
    return 0;
}




/**
 * Read input from command line
 */
void read_input(char* buffer, int size)
{
    char *nl = NULL;
    memset(buffer, 0, size);
    
    if ( fgets(buffer, size, stdin) != NULL ) {
        nl = strchr(buffer, '\n');
        if (nl) *nl = '\0'; // truncate, ovewriting newline
    }
}

struct request_packe{
    
};


//from another project
static size_t size_packet = sizeof(struct packet);

void set0(struct packet* p)
{
    memset(p, 0, sizeof(struct packet));
}

struct packet* ntohp(struct packet* np)
{
    struct packet* hp = (struct packet*) malloc(size_packet);
    memset(hp, 0, size_packet);
    
    hp->conid = ntohs(np->conid);
    hp->type = ntohs(np->type);
    hp->comid = ntohs(np->comid);
    hp->datalen = ntohs(np->datalen);
    memcpy(hp->buffer, np->buffer, LENBUFFER);
    
    return hp;
}

struct packet* htonp(struct packet* hp)
{
    struct packet* np = (struct packet*) malloc(size_packet);
    memset(np, 0, size_packet);
    
    np->conid = ntohs(hp->conid);
    np->type = ntohs(hp->type);
    np->comid = ntohs(hp->comid);
    np->datalen = ntohs(hp->datalen);
    memcpy(np->buffer, hp->buffer, LENBUFFER);
    
    return np;
}

void printpacket(struct packet* p, int ptype)
{
    if(!DEBUG)
        return;
    
    if(ptype)
        printf("\t\tHOST PACKET\n");
    else
        printf("\t\tNETWORK PACKET\n");
    
    printf("\t\tconid = %d\n", p->conid);
    printf("\t\ttype = %d\n", p->type);
    printf("\t\tcomid = %d\n", p->comid);
    printf("\t\tdatalen = %d\n", p->datalen);
    printf("\t\tbuffer = %s\n", p->buffer);
    
    fflush(stdout);
}



int read_reply(){
    
    
    //    while(length = recv(sock_control, buffer, sizeof(buffer), 0))
    //    {
    //
    //
    //    }
    char retcode[512];
    
    //int retcode = 0;
    if (recv(sock_control, &retcode, sizeof retcode, 0) < 0) {
        perror("client: error reading message from server\n");
        return -1;
    }
    printf("%s",retcode);
    char* ret=strtok(retcode," ") ;
    
    //strcpm(ret,retcode,3);
    
    int re=atoi(retcode);
    bzero(retcode, sizeof(retcode));
    return re;
}

int read_reply2(){
    
    char retcode[512];
    
    //int retcode = 0;
    if (recv(sock_control, &retcode, sizeof retcode, 0) < 0) {
        perror("client: error reading message from server\n");
        return -1;
    }
    char ret[5];
    strncpy(ret,retcode,3);
    //printf()
    int response=atoi(ret);
    bzero(retcode, sizeof(retcode));
    return response;
}
//void read_reply_loop(){
//    char retcode[512];
//    int numbytes;
//    //int retcode = 0;
//    while ((numbytes = recv(sock_control, &retcode, sizeof retcode, 0)) > 0) {
//        retcode[numbytes]="\0";
//    }
//}

/**
 * Print response message
 */
void print_reply(int rc)
{
    switch (rc) {
        case 220:
            printf("220 Welcome, server ready.\n");
            break;
        case 221:
            printf("221 Goodbye!\n");
            break;
        case 226:
            printf("226 Closing data connection. Requested file action successful.\n");
            break;
        case 331:
            printf("331 User name okay\n");
            break;
        case 550:
            printf("550 Requested action not taken. File unavailable.\n");
            break;
    }
    
}


/**
 * Parse command in cstruct list
 */
int ftclient_read_command(char* buf, int size, struct command *cstruct)
{
    memset(cstruct->code, 0, sizeof(cstruct->code));
    memset(cstruct->arg, 0, sizeof(cstruct->arg));
    
    //    printf("ftclient> ");    // prompt for input
    //    fflush(stdout);
    //
    //
    //
    //
    //
    //    // wait for user to enter a command
    read_input(buf, size);
    
    //active or passive
    
    char changename[40];
    char *empty=NULL;
    char *arg = NULL;
    arg = strtok (buf," ");
    arg = strtok (NULL, " ");
   // printf("arg:%s",arg);
    if (strcmp(buf, "rename")==0){
        
        char temp1[50];
        char temp2[50];
        strcpy(temp1, arg) ;
        arg = strtok (NULL, " ");
        strcpy(temp2, arg) ;
        
        sprintf(changename,"%s %s",temp1,temp2);
    }
    int i=0;
    if(strcmp(buf,"get")==0){
        char temp1[50];
        char temp2[50];
        strcpy(temp1, arg);
        strncpy(cstruct->arg, arg, strlen(arg));
        strcat(cstruct->arg, "\r\n");
      //  printf("c->arg:%s",cstruct->arg);
        arg = strtok (NULL, " ");
      //  printf("arg:%s",arg);
        if (arg!=NULL) {
            strcpy(temp2, arg) ;
            i=3;
            sprintf(changename,"RETR %s %s\r\n",temp1,temp2);
        }
        
    }
    if(strcmp(buf,"reget")==0){
        int n=0;
        char BUFF[2048];
        int position=0;
        char *arg1 = NULL;
        arg1 = strtok (buf," ");
        char address[256];
       // printf("arg:%s",arg);
        if (arg!=NULL) {
            sprintf(address,"%s",arg);
       //     printf("\naddress is :%s",address);
            int fd=open(address,O_RDWR);
            while((n=read(fd,BUFF,2048))>0){
                position=position+n;
            }
       //     printf("\n%d\n",position);
            sprintf(changename, "%d  %s",position,arg);
            
        }
        
    }
    
    if(strcmp(buf,"put")==0){
        char temp1[50];
        char temp2[50];
        strcpy(temp1, arg) ;
        strncpy(cstruct->arg, arg, strlen(arg));
        strcat(cstruct->arg, "\r\n");
     //   printf("c->arg:%s",cstruct->arg);
        arg = strtok (NULL, " ");
     //   printf("arg:%s",arg);
        if (arg!=NULL) {
            strcpy(temp2, arg) ;
            i=3;
            sprintf(changename,"STOR %s %s\r\n",temp1,temp2);
        }
        
    }
    
    if (arg != NULL){
        
        // store the argument if there is one
        strncpy(cstruct->arg, arg, strlen(arg));
        strcat(cstruct->arg, "\r\n");
    }else if(arg==NULL&&strcmp(buf,"get")!=0&&strcmp(buf, "put")!=0){
        strcpy(cstruct->arg, "\r\n");
    }
    
    // buf = command
    if (strcmp(buf, "ls") == 0) {
        strcpy(cstruct->code, "LIST");
    }else if (strcmp(buf, "reget") == 0) {
        strcpy(cstruct->code, "REST");    }
    else if (strcmp(buf, "get") == 0) {
        strcpy(cstruct->code, "RETR");
    }else if (strcmp(buf, "put") == 0) {
        strcpy(cstruct->code, "STOR");
    }
    
    else if (strcmp(buf, "quit") == 0) {
        strcpy(cstruct->code, "QUIT");
    }else if (strcmp(buf, "cd") == 0){
        strcpy(cstruct->code, "CWD");
    }else if(strcmp(buf, "mkdir") == 0){
        strcpy(cstruct->code, "MKD");
    }else if(strcmp(buf, "pwd") == 0){
        strcpy(cstruct->code, "PWD");
    }else if(strcmp(buf, "delete") == 0){
        strcpy(cstruct->code, "DELE");
    }else if(strcmp(buf, "rename") == 0){
        strcpy(cstruct->code, "RNFR");
    }else if(strcmp(buf, "ascii") == 0){
        strcpy(cstruct->code, "TYPE A");
    }else if(strcmp(buf, "binary") == 0){
        strcpy(cstruct->code, "TYPE I");
    }else if (strcmp("passive",buf)==0) {
        count++;
	strcpy(cstruct->code,"passive");
        if (count%2==1) {
            strcpy(mode, "passive");
	

            printf("passive mode: on");
        }else if (count%2==0){
            strcpy(mode, "active");
            printf("passive mode: off");
	   
        }
    }else if (strcmp("constrain",buf)==0) {
        ccount++;
        strcpy(cstruct->code,"constrain");
        if (ccount%2==1) {
            strcpy(transmit, "on");
            printf("constrain mode: on");
        }else if (ccount%2==0){
            strcpy(transmit, "off");
            printf("constrain mode: off");
        }
    }
    else {//invalid
        return -1;
    }
    
    // store code in beginning of buffer
    
    
    char buffer[270];
    if(strcmp(cstruct->code, "RNFR")==0){
        
        strcpy(buf, changename);
        
        
    }else if (strcmp(cstruct->code, "RETR")==0) {
        if (i==3) {
            strcpy(buf, changename);
        }else{
            sprintf(buffer, "%s %s", cstruct->code, cstruct->arg);
            printf("buffer:%s",buffer);
            strcpy(buf, buffer);
        }
        
    }else if (strcmp(cstruct->code, "STOR")==0) {
        if (i==3) {
            strcpy(buf, changename);
        }else{
            sprintf(buffer, "%s %s", cstruct->code, cstruct->arg);
            printf("buffer:%s",buffer);
            strcpy(buf, buffer);
        }
        
    }else if (strcmp(cstruct->code, "REST")==0) {
        strcpy(buf, changename);
        
        
    }else  {
        sprintf(buffer, "%s %s", cstruct->code, cstruct->arg);
        strcpy(buf, buffer);
        
    
}  // printf("code:%s",cstruct->code);
  //  printf("command:%ss",buffer);
    return 0;
}



/**
 * Do get <filename> command
 */
int ftclient_get(int data_sock, int sock_control, char* arg)
{
    char data[MAXSIZE];
    ssize_t size;
    FILE* fd = fopen(arg, "w");
    
    while ((size = recv(data_sock, data, MAXSIZE, 0)) > 0) {
        fwrite(data, 1, size, fd);
    }
    
    if (size < 0) {
        perror("error\n");
    }
    
    fclose(fd);
    return 0;
}


/**
 * Open data connection
 */
int ftclient_open_conn(int sock_con)
{
    int sock_listen = socket_create(CLIENT_PORT_ID);
    
    // send an ACK on control conn
    int ack = 1;
    if ((send(sock_con, (char*) &ack, sizeof(ack), 0)) < 0) {
        printf("client: ack write error :%d\n", errno);
        exit(1);
    }
    
    int sock_conn = socket_accept(sock_listen);
    close(sock_listen);
    return sock_conn;
}

int ftclient_open_pasvconn(char* host,char* port){
    int s;
    
    struct addrinfo hint, *res, *rp;
    
    
    
    
    
    // Get matching addresses
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    
    s = getaddrinfo(host, port, &hint, &res);
    if (s != 0) {
        printf("getaddrinfo() error %s", gai_strerror(s));
        exit(1);
    }
    
    
    
    
    int sock_data=0;
    // Find an address to connect to & connect
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        sock_data = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        
        if (sock_control < 0)
            continue;
        
        if(connect(sock_data, res->ai_addr, res->ai_addrlen)==0) {
            break;
        } else {
            perror("connecting stream socket");
            exit(1);
        }
        
    }
    char datastr[512];
    recv(sock_data, &datastr, sizeof(datastr) ,0 );
    return 1;
}
//active mode
int conn_active(char*host){
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    
    client_addr.sin_port = htons(Active_port);
    
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }
    
    
    if (bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr))<0)
    {
        printf("Client Bind Port Failed!\n");
        exit(1);
    }
    
    
    struct sockaddr_in  server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    
    if(listen(client_socket, SOMAXCONN)<0){
        perror("listen error");
    }
    
    send_activesss();
    usleep(1000*sleep_time);
    read_reply();
    
    char requst[6]="LIST\r\n";
    if(send(sock_control, requst, sizeof(requst), 0)<0){
        perror("send in 347 fail:");
    }
  //  read_reply();
    if (inet_aton(Remotehost, &server_addr.sin_addr) == 0)
    {
        printf("Server IP Address Error!\n");
        exit(1);
    }
    
    server_addr.sin_port = htons(Active_port);
    socklen_t server_addr_length = sizeof(server_addr);
//printf("into accept");
    int newsock=accept(client_socket, (struct sockaddr*)&server_addr,  &server_addr_length);//zheli要改
    if (newsock<0) {
        perror("accept error:");
        
    }
//printf("into recv");
    char buff[BUFFER_SIZE];
    bzero(buff, sizeof(buff));
    
    //  send(client_socket, buff, BUFFER_SIZE, 0);
    
    
    
   // bzero(buff, sizeof(buff));
    int length = 0;
    int transferSize=0;
	//printf("into 675\n");
    while (recv(newsock, buff, BUFFER_SIZE, 0) != 0){
        
        transferSize += BUFFER_SIZE;
        printf("%s", buff);
        bzero(buff, sizeof(buff));
    }
    
    
    close(client_socket);
    close(newsock);
    
    return 1;
}
int conn_active_for_file(char command[20],char*fileName){
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    
    client_addr.sin_port = htons(Active_port);
    
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }
    
    
    if (bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr))<0)
    {
        printf("Client Bind Port Failed!\n");
        exit(1);
    }
    
    
    struct sockaddr_in  server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    
    if(listen(client_socket, SOMAXCONN)<0){
        perror("listen error");
    }
    
    send_activesss();
    usleep(1000*sleep_time);
    read_reply();//200
    
   // printf("command:%s",command);
    // printf("%d",sizeof(command));
    if(send(sock_control, command,  strlen(command), 0)<0){
        perror("send in 347 fail:");
    }
    int rect=read_reply();//send RETR
    if (rect!=550&&rect!=511){
        if (inet_aton(Remotehost, &server_addr.sin_addr) == 0)
        {
            printf("Server IP Address Error!\n");
            exit(1);
        }
        
        server_addr.sin_port = htons(Active_port);
        socklen_t server_addr_length = sizeof(server_addr);
        
        int newsock=accept(client_socket, (struct sockaddr*)&server_addr,  &server_addr_length);//zheli要改
        if (newsock<0) {
            perror("accept error:");
            
        }
        
        char buff[BUFFER_SIZE];
        bzero(buff, sizeof(buff));
        
        //  send(client_socket, buff, BUFFER_SIZE, 0);
        
        
        
        bzero(buff, sizeof(buff));
        int length = 0;
        int transferSize=0;
        int file=0;
        fileName[strlen(fileName)-1]=0;
        fileName[strlen(fileName)-1]=0;
      //  printf("fileName Store in:%s",fileName);
        file = open(fileName, O_RDWR | O_CREAT,0777);
        if (file<=0) {
            perror("open fail:");
        }
        
        while ((recv(newsock, buff, BUFFER_SIZE, 0)) != 0) {
            
            if ( write(file, buff, BUFFER_SIZE)<0) {
                perror("write error");
            }
            //printf("%s",buff);
            bzero(buff, sizeof(buff));
            transferSize += length;
        }
        printf("sucess");
       
        
        close(file);
        close(client_socket);
        
        
        
        
        
        
        close(newsock);}
    
    return 1;
}
int conn_for_regetfile(char* host, int port,char* fileName){
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    
    client_addr.sin_port = htons(0);
    
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }
    
    
    if (bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)))
    {
        printf("Client Bind Port Failed!\n");
        exit(1);
    }
    
    
    struct sockaddr_in  server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    
    
    if (inet_aton(Remotehost, &server_addr.sin_addr) == 0)
    {
        printf("Server IP Address Error!\n");
        exit(1);
    }
    
    server_addr.sin_port = htons(port);
    socklen_t server_addr_length = sizeof(server_addr);
    
    
    if (connect(client_socket, (struct sockaddr*)&server_addr, server_addr_length) < 0)
    {
        printf("Can Not Connect To %s!\n", Remotehost);
        exit(1);
    }
    
    FILE *fp =fopen(fileName,"a+");
    if(fp==0){printf("can't open file\n");
        return 0;
    }
    fseek(fp,0,SEEK_END);
    char buff[50];
    
    
    int transferSize = 0, recvLen;
    
    
    while ((recvLen = recv(client_socket, buff, 50, 0)) != 0) {
        
        
        fwrite(buff, recvLen,1,fp);
        
        transferSize += recvLen;
        
    }
    printf("sucess");
    fclose(fp);
    close(client_socket);
    
    
    return 1;
}
int conn(char* host, int port){
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    
    client_addr.sin_port = htons(0);
    
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }
    
    
    if (bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)))
    {
        printf("Client Bind Port Failed!\n");
        exit(1);
    }
    
    
    struct sockaddr_in  server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    
    
    if (inet_aton(Remotehost, &server_addr.sin_addr) == 0)
    {
        printf("Server IP Address Error!\n");
        exit(1);
    }
    
    server_addr.sin_port = htons(port);
    socklen_t server_addr_length = sizeof(server_addr);
    
    
    if (connect(client_socket, (struct sockaddr*)&server_addr, server_addr_length) < 0)
    {
        printf("Can Not Connect To %s!\n", Remotehost);
        exit(1);
    }
    
    
    
    char buff[BUFFER_SIZE];
    bzero(buff, sizeof(buff));
    
    //  send(client_socket, buff, BUFFER_SIZE, 0);
    
    
    
    bzero(buff, sizeof(buff));
    int length = 0;
    int transferSize=0;
    while (recv(client_socket, buff, BUFFER_SIZE, 0) != 0){
        
        transferSize += BUFFER_SIZE;
        printf("%s", buff);
        bzero(buff, sizeof(buff));
    }
    close(client_socket);
    
    
    return 1;
}
int conn_for_three_get(char* host, int port,char* fileName){
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    
    client_addr.sin_port = htons(0);
    
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }
    
    
    if (bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)))
    {
        printf("Client Bind Port Failed!\n");
        exit(1);
    }
    
    
    struct sockaddr_in  server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    
    
    if (inet_aton(Remotehost, &server_addr.sin_addr) == 0)
    {
        printf("Server IP Address Error!\n");
        exit(1);
    }
    
    server_addr.sin_port = htons(port);
    socklen_t server_addr_length = sizeof(server_addr);
    
    
    if (connect(client_socket, (struct sockaddr*)&server_addr, server_addr_length) < 0)
    {
        printf("Can Not Connect To %s!\n", Remotehost);
        exit(1);
    }
    return client_socket;
}
int wirte_file(int client_socket,char*fileName){
    int file=0;
    file = open(fileName, O_RDWR | O_CREAT,0777);
    
    char buff[50];
    
    
    int transferSize = 0, recvLen;
    while ((recvLen = recv(client_socket, buff, 50, 0)) != 0) {
        
        if ( write(file, buff, recvLen)<0) {
            perror("write error");
        }
        transferSize += recvLen;
    }
    printf("sucess");
    close(file);
    close(client_socket);
    return 1;
}

void constrain(clock_t time,int byte){
    double speed;
    clock_t start, end;
    start=clock();
   // printf("into constrain\n");
  
        clock_t t1, t2;
        int delay = ((double)byte/transmit_rate - time/CLOCKS_PER_SEC);
        
       //printf("delay:%d\n",delay);
    
            t1 = clock();
            
           usleep(delay*CLOCKS_PER_SEC);
            t2 = clock();
           // printf("t2-t1:%Lf\n",(long double)(t2-t1)/CLOCKS_PER_SEC);
    
    
}

int conn_for_file(char* host, int port,char* fileName){
    //建立TCP连接
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    
    client_addr.sin_port = htons(0);
    
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }
    
    
    if (bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)))
    {
        printf("Client Bind Port Failed!\n");
        exit(1);
    }
    
    
    struct sockaddr_in  server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    
    
    if (inet_aton(Remotehost, &server_addr.sin_addr) == 0)
    {
        printf("Server IP Address Error!\n");
        exit(1);
    }
    
    server_addr.sin_port = htons(port);
    socklen_t server_addr_length = sizeof(server_addr);
    
    
    if (connect(client_socket, (struct sockaddr*)&server_addr, server_addr_length) < 0)
    {
        printf("Can Not Connect To %s!\n", Remotehost);
        exit(1);
    }
    //连接成功
    
    char remotestr[100];
    if(recv(sock_control, &remotestr, sizeof remotestr, 0) < 0  ){
        perror("client: error reading message from server\n");
        
    }
  //  printf("the remotestr is :%s",remotestr);
    
    char ret[5];
    strncpy(ret,remotestr,3);
    int resp=atoi(ret);
    int num;
    
    char filennn[100];
    if(sscanf(remotestr, "150 Opening ASCII mode data connection for %s (%d bytes).\n",filennn,&num)){
      //  printf("the test result:%d\n",num);
        
    }
    else if(sscanf(remotestr, "150 Opening BINARY mode data connection for %s (%d bytes).\n",filennn,&num)){
      //  printf("the test result:%d\n",num);
        
    }
  //  else printf("some error here\n");
    
    int file=0;
    file = open(fileName, O_RDWR | O_CREAT,0777);
    if (file<=0) {
        perror("open fail:");
    }
    //printf("filename%s\n",fileName);
    char buff[BUFFER_SIZE];
    
    
    int transferSize = 0, recvLen;
    int total=0;
    int index=0;
    total=num;
    char str[total+1];
    memset(str,'\0',sizeof(str));
    char tmp[5] = {'-','\\','|','/','\0'};
    clock_t startall;
    clock_t endall;
    startall=clock();
    while (1) {
        clock_t start,end;
        start =clock();
        int recvLen = recv(client_socket, buff, BUFFER_SIZE, 0);
        end=clock();
        if(recvLen != 0){
            //写
            if ( write(file, buff, recvLen)<0) {
                perror("write error");}
           // if(total<100){printf("\033[47;%dm]Done",30);}
            while(index<=total&&(5*index)<=100 ){
                str[index]='#';
                if(index<=(total/2))
                    printf(RED"%s[%d%%%c]\r"NONE,str,index*5,tmp[index%4]);
                else
                    printf(BLUE"%s[%d%%%c]\r"NONE,str,index*5,tmp[index%4]);
                fflush(stdout);
                index++;
                usleep(100000);
            }
            transferSize += recvLen;
            
            bzero(buff, sizeof(buff));
            
            
            clock_t time;
            time=end-start;
            // printf("time: %f\n", (double)time/CLOCKS_PER_SEC);
            if (strcmp(transmit,"on")==0) {
                constrain(time, recvLen);
            }
        }
        else{
            break;
        }
        
    }
    printf("sucess\n");
    endall=clock();
    printf("trains speed:%ldKB/s",num*CLOCKS_PER_SEC/((endall-startall)*1000));
    close(file);
    close(client_socket);
    
    
    return 1;
}

int conn_for_put(char* host, int port,int fd){
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    
    client_addr.sin_port = htons(0);
    
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }
    
    
    if (bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)))
    {
        printf("Client Bind Port Failed!\n");
        exit(1);
    }
    
    
    struct sockaddr_in  server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    
    
    if (inet_aton(Remotehost, &server_addr.sin_addr) == 0)
    {
        printf("Server IP Address Error!\n");
        exit(1);
    }
    
    server_addr.sin_port = htons(port);
    socklen_t server_addr_length = sizeof(server_addr);
    
    
    if (connect(client_socket, (struct sockaddr*)&server_addr, server_addr_length) < 0)
    {
        printf("Can Not Connect To %s!\n", Remotehost);
        exit(1);
    }
    
    
    
    
    char buff[50];
    
    
    int transferSize = 0, recvLen;
    long sendLen;
    //set a clock
    while ((sendLen = read(fd, buff, 50)) != 0){
        
        send(client_socket, buff, sendLen, 0);
        transferSize += sendLen;
    }
    printf("sucess");
    close(fd);
    close(client_socket);
    
    
    return 1;
}


/**
 * Input: cmd struct with an a code and an arg
 * Concats code + arg into a string and sends to server
 */
int ftclient_send_cmd(struct command *cmd)
{
    char buffer[MAXSIZE];
    long rc;
    
    sprintf(buffer, "%s %s", cmd->code, cmd->arg);
    
    // Send command string to server
    rc = send(sock_control, buffer, (int)strlen(buffer), 0);
    if (rc < 0) {
        perror("Error sending command to server");
        return -1;
    }
    
    return 0;
}

int ftclient_send_cmd2(char* cmd)
{
    char buffer[MAXSIZE];
    int rc;
    
    sprintf(buffer, "%s", cmd);
    
    // Send command string to server
    rc = send(sock_control, buffer, (int)strlen(buffer), 0);
    if (rc < 0) {
        perror("Error sending command to server");
        return -1;
    }
    
    return 0;
}

/**
 * Get login details from user and
 * send to server for authentication
 */
void ftclient_login()
{
    struct command cmd;
    char user[256];
    memset(user, 0, 256);
    
    // Get username from user
    printf("Name: ");
    fflush(stdout);
    
    read_input(user, 256);
    strcat(user, "\r\n");
    
    // Send USER command to server
    strcpy(cmd.code, "USER");
    strcpy(cmd.arg, user);
    ftclient_send_cmd(&cmd);
    
    // Wait for go-ahead to send password
    int wait;
    recv(sock_control, &wait, sizeof wait, 0);
    read_reply();
    
    // Get password from user
    fflush(stdout);
    char *pass = getpass("Password: ");
    
    // Send PASS command to server
    strcat(pass, "\r\n");
    strcpy(cmd.code, "PASS");
    strcpy(cmd.arg, pass);
    ftclient_send_cmd(&cmd);
    
    // wait for response
    int retcode = read_reply();
    //read_reply_loop();
    //
    //    strcpy(cmd.code, "SYST");
    //    strcpy(cmd.arg,"\r\n");
    //    ftclient_send_cmd(&cmd);
    //    read_reply2();
    //
    //
    //    if (strcmp(Remotehost, "10.3.255.85")!=0) {
    //        strcpy(cmd.code, "FEAT");
    //
    //        strcpy(cmd.arg,"\r\n");
    //        ftclient_send_cmd(&cmd);
    //        read_reply2();
    //    }
    //
    //
    //    strcpy(cmd.code, "PWD");
    //    strcpy(cmd.arg,"\r\n");
    //    ftclient_send_cmd(&cmd);
    //    read_reply2();
    // read_reply2();
    //printf("%d",retcode);
    switch (retcode) {
        case 430:
            printf("Invalid username/password.\n");
            exit(0);
        case 230:
            printf("Successful login.\n");
            break;
        default:
            perror("error reading message from server");
            exit(1);
            break;
    }
}


// send epsv
int send_pasv(){
    char passive[20]="PASV\r\n";
    
    if(send(sock_control, passive, (int)strlen(passive),0) < 0 ) {
        close(sock_control);
        exit(1);
    }
    //int retcode = 0;
    char passivestr[512];
    if (recv(sock_control, &passivestr, sizeof passivestr, 0) < 0) {
        perror("client: error reading message from server\n");
        return -1;
    }
    
    printf("the passivestr: %s", passivestr);
    int fetched_num;
    int d1,d2,d3,d4,d5,d6;
    //    if(sscanf(passivestr, "229 Entering Extended Passive Mode (|||%d|)\n", &fetched_num)){
    //        printf("the test result: %d\n", fetched_num);
    //    }
    if(sscanf(passivestr, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\n", &d1,&d2,&d3,&d4,&d5,&d6)){
        
    }
    else
        
        printf("some error here\n");
    fetched_num=d5*256+d6;
    return fetched_num;
}

//send active
void send_activesss(){
    char passive[40];
    sprintf(passive, "EPRT |1|%s|%d|\r\n",Host,Active_port);
    if(send(sock_control, passive, (int)strlen(passive),0) < 0 ) {
        close(sock_control);
        exit(1);
    }
    
    
    
}


void cmd_ls(char*buffer){
    //passive mode
    if(strcmp(mode, "passive")==0){
        int data_port=send_pasv();
        usleep(1000*500);
        //printf("data:%d\n",data_port);
        
        if (send(sock_control, "LIST\r\n", (int)strlen("LIST\r\n"), 0) < 0 ) {
            close(sock_control);
            exit(1);
        }
        //read_reply();
        if (conn(Remotehost,data_port) <= 0) {
            perror("Error opening socket for data connection");
            exit(1);
        }
        //read_reply();
    }else {
        usleep(1000*sleep_time);
        //send_active();
        conn_active(Remotehost);
        usleep(1000*sleep_time);
        read_reply();
    }
    //active mode
    
}
void cmd_cwd(char*buffer){
    
    if (send(sock_control, buffer, (int)strlen(buffer), 0) < 0 ) {
        close(sock_control);
        exit(1);
    }
    // read_reply();
}
void cmd_binary(char*buffer){
  //  printf("into binary");
    if (send(sock_control, "TYPE I\r\n",8, 0) < 0 ) {
        close(sock_control);
        exit(1);
    }
    // read_reply();
}
void cmd_ascii(char*buffer){
    
    if (send(sock_control, "TYPE A\r\n", 8, 0) < 0 ) {
        close(sock_control);
        exit(1);
    }
    // read_reply();
}
void cmd_mkdir(char*buffer){
    if (send(sock_control, buffer, (int)strlen(buffer), 0) < 0 ) {
        close(sock_control);
        exit(1);
    }
    // read_reply();
}
void cmd_quit(char*buffer){
    if (send(sock_control, buffer, (int)strlen(buffer), 0) < 0 ) {
        close(sock_control);
        exit(1);
    }
    //read_reply();
    exit(1);
}
void cmd_rename(char*code,char* buf){
   // printf("buf:%s",buf);
    char* arg=NULL;
    char code1[255];
    char code2[255];
    arg = strtok (buf," ");
    arg = strtok (NULL, " ");
    char temp1[50];
    char temp2[50];
    strcpy(temp1, arg) ;
    
    
    sprintf(code1,"%s %s\r\n",code,buf);
  //  printf("buf:%s", buf);
    //printf("code:%s", code1)
    ;
    sprintf(code2,"%s %s\r\n","RNTO",temp1);
    if (send(sock_control, code1, (int)strlen(code1), 0) < 0 ) {
        close(sock_control);
        exit(1);
    }
    //  read_reply();
    
    
    if (send(sock_control, code2, (int)strlen(code2), 0) < 0 ) {
        close(sock_control);
        exit(1);
    }
    //   read_reply();
    
}
//reget
void cmd_reget(char*code,char* buf){
  //  printf("buf:%s",buf);
    
    char *arg1;
    char *arg2;
    char code1[255];
    char code2[255];
    arg1 = strtok (buf," ");
    sprintf(code1,"REST %s\r\n",arg1);
    arg2= strtok(NULL," ");
    // printf("\n code2is:%s",arg2);
    sprintf(code2,"RETR %s\r\n",arg2);
    char *res=NULL;
    char temp[5][20];
    char tem[40];
    strcpy(tem, code2);
    int i=0;
    res=strtok(tem, " ");
    while (res!=NULL) {
        
        strcpy(temp[i],res);
        i++;
        res = strtok( NULL, " ");
        
    }
  //  printf("i:%d\n",i);
    int data_port=send_pasv();
    //sleep(0.5);
    if(i==2){
        if (send(sock_control, code1, (int)strlen(code1), 0) < 0 ) {
            close(sock_control);
            exit(1);
        }
        
        if (send(sock_control, code2, (int)strlen(code2), 0) < 0 ) {
            close(sock_control);
            exit(1);
        }
        
        
        if (read_reply2()!=551) {
            char *arg = NULL;
            char buf[40];
            strcpy(buf, code2);
            arg = strtok (buf," ");
            arg = strtok (NULL, " ");
            char temp[50];
            strcpy(temp, Filelocate);
            strcat(temp, arg);
            temp[strlen(temp)-1]=0;
            temp[strlen(temp)-1]=0;
            
            
            if (conn_for_regetfile(Host, data_port, temp) <= 0) {
                perror("Error opening socket for data connection");
                exit(1);
            }
            
            
            // read_reply();
        }
        
    }
    
    
    read_reply();
    
}
void cmd_get(char*buffer){
    char *res=NULL;
    char temp[5][255];
    char tem[255];
    strcpy(tem, buffer);
    
    int i=0;
    res=strtok(tem, " ");
    while (res!=NULL) {
        
        strcpy(temp[i],res);
        i++;
        res = strtok( NULL, " ");
        
    }
   // printf("i:%d\n",i);
    if(strcmp(mode, "passive")==0){
        
        
        
        if(i==2){
            char *arg = NULL;
            char buf[255];
            strcpy(buf, buffer);
            arg = strtok (buf," ");
            arg = strtok (NULL, " ");
            char temp[255];
            strcpy(temp, Filelocate);
            strcat(temp, arg);
            temp[strlen(temp)-1]=0;
            temp[strlen(temp)-1]=0;
            //
            
            //
            char filen[255];
            sprintf(filen, "SIZE %s\r\n",temp);
            if (send(sock_control, filen, (int)strlen(buffer), 0) < 0 ) {
                close(sock_control);
                exit(1);
            }
            read_reply();
            int data_port=send_pasv();
            usleep(1000*sleep_time);
            if (send(sock_control, buffer, (int)strlen(buffer), 0) < 0 ) {
                close(sock_control);
                exit(1);
            }
            
           // printf("into:1289\n");
            
            //  if (resp!=551&&resp!=550) {
           // printf("into:1291\n");
            
            
            
            
            if (conn_for_file(Remotehost, data_port, temp) <= 0) {
                perror("Error opening socket for data connection");
                exit(1);
            }
            
            
            //    read_reply();
            //   }
            
        }else if(i==3){
            
            char local[255],remote[255];
            int data_port=send_pasv();
            usleep(1000*sleep_time);
            sprintf(remote, "RETR %s\r\n", temp[1]);//send
            if (send(sock_control, remote, (int)strlen(remote), 0) < 0 ) {
                close(sock_control);
                exit(1);
            }
            
            //int rect=read_reply();
            //       if (rect!=550&&rect!=511){
            temp[2][strlen(temp[2])-1]=0;
            temp[2][strlen(temp[2])-1]=0;
          //  printf("temp2:%s",temp[2]);
            
            strcpy(local, Filelocate);
            strcat(local, temp[2]);
            //conn
            usleep(1000*sleep_time);
            if (conn_for_file(Remotehost, data_port, local) <= 0) {
                perror("Error opening socket for data connection");
                exit(1);
            }
            
            //        }
            //   read_reply();
        }else {
            printf("551 File not available\n");
        }
        
    }else if (strcmp("active", mode)==0){
        if(i==2){
            char *arg = NULL;
            char buf[255];
            strcpy(buf, buffer);
            arg = strtok (buf," ");
            arg = strtok (NULL, " ");
            char temp[50];
            strcpy(temp, Filelocate);
            strcat(temp, arg);
          //  printf("buffer in active:%s",buffer);
            conn_active_for_file(buffer,temp);
            //   read_reply();
            
        }
        else if (i==3){
            char local[255],remote[255];
            
            sprintf(remote, "RETR %s\r\n", temp[1]);//send
            
            
            temp[2][strlen(temp[2])-1]=0;
            temp[2][strlen(temp[2])-1]=0;
           // printf("temp2:%s",temp[2]);
            
            strcpy(local, Filelocate);
            strcat(local, temp[2]);
            //conn
            usleep(1000*sleep_time);
            conn_active_for_file(remote, local);
        }
        
    }
}
void cmd_put(char*buffer){
    int i=0;
    char *res=NULL;
    char temparray[5][20];
    char tem[255];
    strcpy(tem, buffer);
    res=strtok(tem, " ");
    while (res!=NULL) {
        
        strcpy(temparray[i],res);
        i++;
        res = strtok( NULL, " ");
        
    }
    //printf("i:%d",i);
    
    if(strcmp(mode, "passive")==0)
    {
        if (i==2) {
            
            printf("into 1107");
            char *arg = NULL;
            char buf[40];
            
            strcpy(buf, buffer);
            arg = strtok (buf," ");
            arg = strtok (NULL, " ");
            char temp[50];
            strcpy(temp, Filelocate);
            strcat(temp, arg);
            temp[strlen(temp)-1]=0;
            temp[strlen(temp)-1]=0;
            int file=0;
            file=open(temp, O_RDONLY);
            if (file<=0) {
                printf("ftp:Can't open `%s': No such file or directory",temp);
            }else{
                int data_port=send_pasv();
                usleep(1000*sleep_time);
                
                if (send(sock_control, buffer, (int)strlen(buffer), 0) < 0 ) {
                    close(sock_control);
                    exit(1);
                }
                //  read_reply();
                if (conn_for_put(Remotehost, data_port, file) <= 0) {
                    perror("Error opening socket for data connection");
                    exit(1);
                }
                //
                // read_reply();
            }
            
        }else if (i==3){
            char local[40],remote[40];
            printf("%s",temparray[2]);
            sprintf(remote, "STOR %s", temparray[2]);//send
            
            
            printf("local:%s",temparray[1]);
            
            strcpy(local, Filelocate);
            strcat(local, temparray[1]);
            //conn
            
            
            int file=0;
            file=open(local, O_RDONLY);
            if (file<=0) {
                printf("ftp:Can't open : No such file or directory");
            }else{
                int data_port=send_pasv();
                usleep(1000*sleep_time);
                if (send(sock_control, remote, (int)strlen(remote), 0) < 0 ) {
                    close(sock_control);
                    exit(1);
                }
                //   read_reply();
                if (conn_for_put(Remotehost, data_port, file) <= 0) {
                    perror("Error opening socket for data connection");
                    exit(1);
                }
                
                
                //   read_reply();
            }
        }
        
    }else{
        
    }
    
    
}



int main(int argc, char* argv[])
{
    int  retcode, s;
    data_sock=0;
    
    char buffer[MAXSIZE];
    struct command cmd;
    struct addrinfo hints, *res, *rp;
    
    if (argc != 3) {
        printf("usage: ./ftclient hostname port\n");
        exit(0);
    }
    
    char *host = argv[1];
    strcpy(Remotehost, argv[1]);
    char *port = argv[2];
    
    // Get matching addresses
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    s = getaddrinfo(host, port, &hints, &res);
    if (s != 0) {
        printf("getaddrinfo() error %s", gai_strerror(s));
        exit(1);
    }
    
    // Find an address to connect to & connect
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        sock_control = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        
        if (sock_control < 0)
            continue;
        
        if(connect(sock_control, res->ai_addr, res->ai_addrlen)==0) {
            break;
        } else {
            perror("connecting stream socket");
            exit(1);
        }
        close(sock_control);
    }
    freeaddrinfo(rp);
    //*
    fd_set        rset;
    int        maxfdp1,nread,nwrite;
    FD_ZERO(&rset);
    maxfdp1 = sock_control + 1;
    //*
    
    // Get connection, welcome messages
    printf("Connected to %s.\n", Remotehost);
    read_reply();
    
    
    
    /* Get name and password and send to server */
    ftclient_login();
    
    while (1) { // loop until user types quit
        //printf("ftclient>");
        FD_SET(sock_control, &rset);
        FD_SET(fileno(stdin), &rset);
        
        if (select(maxfdp1, &rset, NULL, NULL, NULL) < 0)
            printf("select error\n");
        
        
        if (FD_ISSET(STDIN_FILENO, &rset)) {
            
            ftclient_read_command(buffer,sizeof buffer, &cmd);
            // execute command
	//	printf("code in 1839:%s",cmd.code);
            if (strcmp(cmd.code, "LIST") == 0) {
                cmd_ls(buffer);
                
            }else if (strcmp(cmd.code, "RETR") == 0) {
                cmd_get(buffer);
                
            }else if (strcmp(cmd.code, "REST") == 0) {
                cmd_reget(cmd.code,buffer);
            }else if (strcmp(cmd.code, "STOR") == 0) {
                cmd_put(buffer);
                
            }else if(strcmp(cmd.code, "CWD") == 0){
                cmd_cwd(buffer);
            }else if(strcmp(cmd.code, "RETR") == 0){
                cmd_cwd(buffer);
            }else if(strcmp(cmd.code, "MKD") == 0){
                cmd_mkdir(buffer);
            }else if(strcmp(cmd.code, "PWD") == 0){
                cmd_cwd(buffer);
            }else if(strcmp(cmd.code, "DELE") == 0){
                cmd_cwd(buffer);
            }else if(strcmp(cmd.code, "RNFR") == 0){
                cmd_rename(cmd.code,buffer);
            }else if(strcmp(buffer, "TYPE A \r\n") == 0){
                cmd_ascii(buffer);
            }else if(strcmp(buffer, "TYPE I \r\n") == 0){
               printf("into TYpe I\n");
		 cmd_binary(buffer);
            }
            else if (strcmp(cmd.code, "QUIT") == 0) {
                cmd_quit(buffer);
                close(sock_control);
                
                
            }else if(strcmp(cmd.code,"passive")==0||strcmp(cmd.code,"constrain")==0){
		
	}else{
	printf("invalid command\n");}
        }
        
        if (FD_ISSET(sock_control, &rset)) {
            
            read_reply();
            
        }
        if (FD_ISSET(data_sock, &rset)){
            
        }
        if (FD_ISSET(active_sock, &rset)) {
            
        }
        
        
        
        
        
        
        
        
    }
    
    // loop back to get more user input
    
    // Close the socket (control connection)
    close(sock_control);
    return 0;
}


