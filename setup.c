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
void AddString(char *String, const char *strfir, const char *strsec){
	strcat(String, strfir);
	strcat(String, " ");
	strcat(String, strsec);
}
void SetUDPServerServerAddr(sockaddr_in *serverAddr){
	bzero(serverAddr, sizeof(*serverAddr));
	serverAddr->sin_family = AF_INET;
	serverAddr->sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr->sin_port = htons(server_port);
}
void SetUDPClientServerAddr(sockaddr_in *serverAddr){
	bzero(serverAddr, sizeof(*serverAddr));
	serverAddr->sin_family = AF_INET;
	serverAddr->sin_port = htons(server_port);
	inet_pton(AF_INET, server, &(serverAddr->sin_addr));
}
void SendHello(const char *username, int sock, const sockaddr *pservaddr, socklen_t servlen){	
	char sendline [MAXSTRINGLENGTH];
	bzero(sendline, MAXSTRINGLENGTH);
	AddString(sendline, HELLO, username);
	int n;
	n = sendto(sock, sendline, strlen(sendline), 0, pservaddr, servlen);
	if(n<0)
		DieWithSystemMessage("sendto() failed");
}
void SetMyClient(MyClient *this, const sockaddr *addrptr, const char *username){
	memcpy(&(this->m_cliaddr), addrptr, sizeof(*addrptr));
	bzero(this->m_username, 128);
	strcpy(this->m_username, username);
	this->m_next = NULL;
	return;
}
void AddMyClient(MyClient *this, MyClient *head){
	this->m_next = head->m_next;
	head->m_next = this;
	return;
}
MyClient *FindMyClient(MyClient *head, const sockaddr *addrptr){
	MyClient *p = head->m_next;
	for(;p!=NULL;p=p->m_next){
		if(memcmp(addrptr, &(p->m_cliaddr), sizeof(*addrptr)) == 0)
			return p;
	}
	return NULL;
}
MyClient *FindMyClientbyName(MyClient *head, const char *name){
	MyClient *p = head->m_next;
	for(;p!=NULL;p=p->m_next){
		if(strcmp(name, p->m_username) == 0)
			return p;
	}
	return NULL;
}
void DeleteMyClient(MyClient *head, const sockaddr *addrptr){
	MyClient *p = head;
	for(;p->m_next!=NULL;p=p->m_next){
		MyClient *pnext = p->m_next;
		if(memcmp(addrptr, &(pnext->m_cliaddr), sizeof(*addrptr)) == 0){
			p->m_next = pnext->m_next;
			free(pnext);
			return;	
		}
	}
	return;
}
void DealName(const char *buffer, char *name, char *msg){
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
