#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
	int cliSocket,n;
	int bufferSize;
	int len = 0;
	int commandLen;
	char sendline[100];
	char recvline[100];
	char fullCommand[4096];
	struct sockaddr_in cliConfig;
	
	printf("%s", argv[1]);
	strcpy(fullCommand, argv[1]);
	if (argc > 2) {
		for (int i = 2; i < argc; i++) {
			strcat(fullCommand, " ");
			strcat(fullCommand, argv[i]);
		}
	}
	
	cliSocket = socket(AF_INET,SOCK_STREAM, 0);
	memset(&cliConfig, 0, sizeof(struct sockaddr_in));
	
	cliConfig.sin_addr.s_addr = inet_addr("127.0.0.1");
	cliConfig.sin_family = AF_INET;
	cliConfig.sin_port = htons(8559);
	
	connect(cliSocket, (struct sockaddr *) &cliConfig, sizeof(cliConfig));
	puts("Connection established!");
	
	bzero(sendline, 100);
	bzero(recvline, 100);
	
	commandLen = strlen(fullCommand);
	
	send(cliSocket, fullCommand, commandLen, 0); //sending command
	recv(cliSocket, &len, sizeof(len), 0);
	recv(cliSocket, recvline, len, 0);
	printf("%s\n", recvline);
	fgets(sendline, 100, stdin); //getting username
	send(cliSocket, sendline, strlen(sendline) + 1, 0);
	recv(cliSocket, &len, sizeof(len), 0);
	recv(cliSocket, recvline, len, 0);
	printf("%s\n", recvline);
	fgets(sendline, 100, stdin); //getting password
	send(cliSocket, sendline, strlen(sendline) + 1, 0);
	
	while (1) {
		//memset(recvline, "\0", sizeof(recvline));
		bzero(recvline, 100);
		recv(cliSocket, &len, sizeof(len), 0);
		recv(cliSocket, recvline, len, 0);
		if (bufferSize == -1) {
			puts("uhoh");
			break;
		}
		if (bufferSize == 0) {
			puts("all done");
			break;
		}
		printf("%s\n", recvline);
	}
}