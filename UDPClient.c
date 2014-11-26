#include "setup.h"

extern const int server_port;
extern  const size_t MAXSTRINGLENGTH;
extern const char *server;
extern const char *END;

void Send2Server(FILE *fp, int sock, const sockaddr *pservaddr, socklen_t servlen){	
	char sendline[MAXSTRINGLENGTH];
	bzero(sendline, MAXSTRINGLENGTH);
	int n;
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
void RecvFromServer(int sock){	
	char recvline[MAXSTRINGLENGTH+1];
	bzero(recvline, MAXSTRINGLENGTH);
	int n;
	n = recvfrom(sock, recvline, MAXSTRINGLENGTH, 0, NULL, NULL);
	if(n<0)
		DieWithSystemMessage("recvfrom() failed");
	fputs(recvline, stdout);
}

int main(int argc, char *argv[]){
	if(argc>2 || argc==1)
		DieWithUserMessage("Parameters", "<Username>");
	
	sockaddr_in servaddr;
	SetUDPClientServerAddr(&servaddr);
	int sock = SetUDPSock();
	SendHello(argv[1], sock, (sockaddr *) &servaddr, sizeof(servaddr));
	RecvFromServer(sock);

	pid_t processID;
	processID = fork();
	if(processID<0)
		DieWithSystemMessage("fork() failed");
	else if(processID>0){
		Send2Server(stdin, sock, (sockaddr *) &servaddr, sizeof(servaddr));
	}else{
		while(1)
			RecvFromServer(sock);
	}
	exit(0);
}