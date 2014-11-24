#include "setup.h"

extern const int server_port;
extern  const size_t MAXSTRINGLENGTH;
extern const char *server;
extern const char *END;

void dg_send(FILE *fp, int sock, const sockaddr *pservaddr, socklen_t servlen){
	int n;
	char sendline[MAXSTRINGLENGTH];
	bzero(sendline, MAXSTRINGLENGTH*sizeof(char));
	while(fgets(sendline, MAXSTRINGLENGTH, fp) != NULL){
		if(strncmp(sendline, END, strlen(END))==0){
			n = sendto(sock, END, strlen(END), 0, pservaddr, servlen);
			if(n<0)
				DieWithSystemMessage("sendto() failed");
			return;
		}
		else{
			n = sendto(sock, sendline, strlen(sendline), 0, pservaddr, servlen);
			if(n<0)
				DieWithSystemMessage("sendto() failed");
		}
	}
}
void dg_recv(int sock){
	int n;
	char recvline[MAXSTRINGLENGTH+1];
	bzero(recvline, MAXSTRINGLENGTH*sizeof(char));
	n = recvfrom(sock, recvline, MAXSTRINGLENGTH, 0, NULL, NULL);
	if(n<0)
		DieWithSystemMessage("recvfrom() failed");
	fputs(recvline, stdout);
}

int main(int argc, char *argv[]){
	if(argc>2 || argc==1)
		DieWithUserMessage("Parameters", "<Username>");
	sockaddr_in servaddr;
	pid_t processID;
	setUDPClientserverAddr(&servaddr);
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock<0)
		DieWithSystemMessage("socket() failed");
	sendHello(argv[1], sock, (sockaddr *) &servaddr, sizeof(servaddr));
	dg_recv(sock);
	processID = fork();
	if(processID<0)
		DieWithSystemMessage("fork() failed");
	else if(processID>0){
		dg_send(stdin, sock, (sockaddr *) &servaddr, sizeof(servaddr));
	}else{
		while(1)
			dg_recv(sock);
	}
	exit(0);
}