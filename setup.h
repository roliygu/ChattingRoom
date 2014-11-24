#ifndef _ROLIY_SETUP
#define _ROLIY_SETUP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

void DieWithUserMessage(const char *msg, const char *detail);
void DieWithSystemMessage(const char *msg);
void strAdd(char *String, const char *strfir, const char *strsec);
void dealwithName(const char *buffer, char *name, char *msg);
void setUDPServerserverAddr(sockaddr_in *serverAddr);
void setUDPClientserverAddr(sockaddr_in *serverAddr);
void sendHello(const char *Username, int sock, const sockaddr *pservaddr, socklen_t servlen);
typedef struct MyClient{
	sockaddr cliaddr;
	char username[128];
	struct MyClient *next;
}MyClient;
void setMyClient(MyClient *this, const sockaddr *addrptr, const char *name);
void addMyClient(MyClient *this, MyClient *head);
MyClient *findMyClient(MyClient *head, const sockaddr *addrptr);
MyClient *findMyClientbyName(MyClient *head, const char *name);
#endif