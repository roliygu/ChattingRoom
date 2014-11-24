#include "setup.h"

const int server_port = 5000;
const size_t MAXSTRINGLENGTH = 1024;
const int MAXCLIENTNUMBER = 128;
const char *server = "127.0.0.1";
const char *HELLO = "@HELLO";
const char *END = "@END";

void DieWithUserMessage(const char *msg, const char *detail){
	fputs(msg, stderr);
	fputs(": ", stderr);
	fputs(detail, stderr);
	fputc('\n', stderr);
	exit(1);
}
void DieWithSystemMessage(const char *msg){
	perror(msg);
	exit(1);
}
void strAdd(char *String, const char *strfir, const char *strsec){
	strcat(String, strfir);
	strcat(String, " ");
	strcat(String, strsec);
}
void setUDPServerserverAddr(sockaddr_in *serverAddr){
	bzero(serverAddr, sizeof(*serverAddr));
	serverAddr->sin_family = AF_INET;
	serverAddr->sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr->sin_port = htons(server_port);
}
void setUDPClientserverAddr(sockaddr_in *serverAddr){
	bzero(serverAddr, sizeof(*serverAddr));
	serverAddr->sin_family = AF_INET;
	serverAddr->sin_port = htons(server_port);
	inet_pton(AF_INET, server, &(serverAddr->sin_addr));
}
void sendHello(const char *Username, int sock, const sockaddr *pservaddr, socklen_t servlen){
	int n;
	char sendline [MAXSTRINGLENGTH];
	strcat(sendline, HELLO);
	strcat(sendline, " ");
	strcat(sendline, Username);
	n = sendto(sock, sendline, strlen(sendline), 0, pservaddr, servlen);
	if(n<0)
		DieWithSystemMessage("sendto() failed");
}
void setMyClient(MyClient *this, const sockaddr *addrptr, const char *name){
	memcpy(&(this->cliaddr), addrptr, sizeof(*addrptr));
	bzero(this->username, 128);
	strcpy(this->username, name);
	this->next = NULL;
	return;
}
void addMyClient(MyClient *this, MyClient *head){
	this->next = head->next;
	head->next = this;
	return;
}
MyClient *findMyClient(MyClient *head, const sockaddr *addrptr){
	MyClient *p = head->next;
	for(;p!=NULL;p=p->next){
		if(memcmp(addrptr, &(p->cliaddr), sizeof(*addrptr)) == 0)
			return p;
	}
	return NULL;
}
MyClient *findMyClientbyName(MyClient *head, const char *name){
	MyClient *p = head->next;
	for(;p!=NULL;p=p->next){
		if(strcmp(name, p->username) == 0)
			return p;
	}
	return NULL;
}
void deleteMyClient(MyClient *head, const sockaddr *addrptr){
	MyClient *p = head;
	for(;p->next!=NULL;p=p->next){
		MyClient *pnext = p->next;
		if(memcmp(addrptr, &(pnext->cliaddr), sizeof(*addrptr)) == 0){
			p->next = pnext->next;
			free(pnext);
			return;	
		}
	}
	return;
}
void dealwithName(const char *buffer, char *name, char *msg){
	size_t bufferlen = strlen(buffer), index=-1,i=0;
	for(;i<bufferlen;i++)
		if(buffer[i]==' '){
			index = i;
			break;
		}
	if(index!=-1){
		strncpy(name, buffer+1, index-1);
		strncpy(msg, buffer+index+1, bufferlen-index);
		size_t msglen = strlen(msg);
		if(msg[msglen-1]!='\n'){
			msg[msglen] = '\n';
			msg[msglen+1] = 0;
		}	
	}else
		strncpy(name, buffer+1, strlen(buffer)-1);
	return;
}
