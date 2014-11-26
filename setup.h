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
void AddString(char *String, const char *strfir, const char *strsec);
void DealName(const char *buffer, char *name, char *msg);
void SetUDPServerServerAddr(sockaddr_in *serverAddr);
void SetUDPClientServerAddr(sockaddr_in *serverAddr);
int SetUDPSock();
void SendHello(const char *username, int sock, const sockaddr *pservaddr, socklen_t servlen);
typedef struct MyClient{
	sockaddr m_cliaddr;
	char m_username[128];
	struct MyClient *m_next;
}MyClient;
void SetMyClient(MyClient *this, const sockaddr *addrptr, const char *name);
void AddMyClient(MyClient *this, MyClient *head);
MyClient *FindMyClient(MyClient *head, const sockaddr *addrptr);
void DeleteMyClient(MyClient *head, const sockaddr *addrptr);
MyClient *FindMyClientbyName(MyClient *head, const char *name);
#endif