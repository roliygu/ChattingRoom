#include <sys/wait.h>
#include "setup.h"

extern const int server_port;
extern  const size_t MAXSTRINGLENGTH;
extern const int MAXCLIENTNUMBER;
extern const char *HELLO;
extern const char *END;

void Send2All(MyClient *headptr, int sock, const char *msg, sockaddr *cliaddrptr, socklen_t len, int flag){
	// flag == 0 means send to all including the sender, else except the sender;
	MyClient *p = headptr->m_next;
	for(;p!=NULL;p=p->m_next){
		if( flag  &&  (memcmp(cliaddrptr, &(p->m_cliaddr), sizeof(*cliaddrptr))==0) )
			continue;
		sendto(sock, msg, strlen(msg), 0, &(p->m_cliaddr), len);
	}
}
void RecvFromClient(int sock, sockaddr *pcliaddr, socklen_t clilen){	
	char recvline[MAXSTRINGLENGTH];
	char sentline[MAXSTRINGLENGTH];

	MyClient headCli, *headCliptr;
	headCliptr = &headCli;
	headCli.m_next = NULL;
	size_t MyCliLen = sizeof(headCli);

	for(;;){
		socklen_t len;
		len = clilen;
		bzero(recvline, MAXSTRINGLENGTH);
		int n = recvfrom(sock, recvline, MAXSTRINGLENGTH, 0, pcliaddr, &len);
		if(n<0)
			DieWithSystemMessage("recvfrom() failed");

		recvline[n] = 0;
		bzero(sentline, MAXSTRINGLENGTH);
		if(strncmp(recvline, HELLO, strlen(HELLO) )== 0){
			MyClient *thisClient = (MyClient *) malloc(MyCliLen);
			SetMyClient(thisClient, pcliaddr, recvline+7);
			AddMyClient(thisClient, headCliptr);				
			AddString(sentline, recvline+7, "is comming.\n");	
			Send2All(headCliptr, sock, sentline, pcliaddr, len, 0);
		}else if(strncmp(recvline, END, strlen(END))== 0){
			MyClient *sender = FindMyClient(headCliptr, pcliaddr);
			AddString(sentline, sender->m_username, "is leaving.\n");
			DeleteMyClient(headCliptr, pcliaddr);
			Send2All(headCliptr, sock, sentline, pcliaddr, len, 0);
		}else{
			MyClient *sender = FindMyClient(headCliptr, pcliaddr);
			if(recvline[0]=='@'){
				char tempname[64], tempmsg[MAXSTRINGLENGTH];
				bzero(tempname, 64);
				bzero(tempmsg, MAXSTRINGLENGTH);
				DealName(recvline, tempname, tempmsg);
				MyClient *recver = FindMyClientbyName(headCliptr ,tempname);
				if(recver!=NULL){
					AddString(sentline, sender->m_username, "to you: ");
					strcat(sentline, tempmsg);
					sendto(sock, sentline, strlen(sentline), 0, &(recver->m_cliaddr), len);
				}
			}else{
				AddString(sentline, sender->m_username, "saying: ");
				strcat(sentline, recvline);
				Send2All(headCliptr, sock, sentline, pcliaddr, len, 1);
			}
		}
	}
}
int main(int argc, char *argv[]){
	sockaddr_in servaddr, cliaddr;
	SetUDPServerServerAddr(&servaddr);
	int sock = SetUDPSock();
	if(bind(sock, (sockaddr *) &servaddr, sizeof(servaddr))<0)
		DieWithSystemMessage("bind() failed");
	RecvFromClient(sock, (sockaddr *) &cliaddr, sizeof(cliaddr));
	return 0;
}