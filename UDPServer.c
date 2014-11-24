#include <sys/wait.h>
#include "setup.h"

extern const int server_port;
extern  const size_t MAXSTRINGLENGTH;
extern const int MAXCLIENTNUMBER;
extern const char *HELLO;
extern const char *END;

void send2All(MyClient *headptr, int sock, const char *msg, sockaddr *cliaddrptr, socklen_t len, int flag){
	MyClient *p = headptr->next;
	for(;p!=NULL;p=p->next){
		if( flag  &&  (memcmp(cliaddrptr, &(p->cliaddr), sizeof(*cliaddrptr))==0) )
			continue;
		sendto(sock, msg, strlen(msg), 0, &(p->cliaddr), len);
	}
}
void dg_recv(int sock, sockaddr *pcliaddr, socklen_t clilen){
	int n;
	socklen_t len;
	char msg[MAXSTRINGLENGTH];
	bzero(msg, MAXSTRINGLENGTH);
	MyClient headCli, *headCliptr;
	headCliptr = &headCli;
	headCli.next = NULL;
	size_t MyCliLen = sizeof(headCli);
	for(;;){
		len = clilen;
		n = recvfrom(sock, msg, MAXSTRINGLENGTH, 0, pcliaddr, &len);
		if(n<0)
			DieWithSystemMessage("recvfrom() failed");
		msg[n] = 0;
		char buffer[MAXSTRINGLENGTH];
		bzero(buffer, MAXSTRINGLENGTH);
		if(strncmp(msg, HELLO, strlen(HELLO) )== 0){
			MyClient *thisClient = (MyClient *) malloc(MyCliLen);
			setMyClient(thisClient, pcliaddr, msg+7);
			addMyClient(thisClient, headCliptr);				
			strAdd(buffer, msg+7, "is comming.\n");	
			send2All(headCliptr, sock, buffer, pcliaddr, len, 0);
		}else if(strncmp(msg, END, strlen(END))== 0){
			MyClient *sender = findMyClient(headCliptr, pcliaddr);
			strAdd(buffer, sender->username, "is leaving.\n");
			deleteMyClient(headCliptr, pcliaddr);
			send2All(headCliptr, sock, buffer, pcliaddr, len, 0);
		}else{
			MyClient *sender = findMyClient(headCliptr, pcliaddr);
			if(msg[0]=='@'){
				char tempname[64], tempmsg[MAXSTRINGLENGTH];
				bzero(tempname, 64);
				bzero(tempmsg, MAXSTRINGLENGTH);
				dealwithName(msg, tempname, tempmsg);
				MyClient *recver = findMyClientbyName(headCliptr ,tempname);
				if(recver!=NULL){
					strAdd(buffer, sender->username, "to you: ");
					strcat(buffer, tempmsg);
					sendto(sock, buffer, strlen(buffer), 0, &(recver->cliaddr), len);
				}
			}else{
				strAdd(buffer, sender->username, "saying: ");
				strcat(buffer, msg);
				send2All(headCliptr, sock, buffer, pcliaddr, len, 1);
			}
		}
	}
}
int main(int argc, char *argv[]){
	sockaddr_in servaddr, cliaddr;
	setUDPServerserverAddr(&servaddr);
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock<0)
		DieWithSystemMessage("socket() failed");
	if(bind(sock, (sockaddr *) &servaddr, sizeof(servaddr))<0)
		DieWithSystemMessage("bind() failed");
	dg_recv(sock, (sockaddr *) &cliaddr, sizeof(cliaddr));
	return 0;
}