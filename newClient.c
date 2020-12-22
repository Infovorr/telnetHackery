/*
	Telnet Client
	
	Attempts to connect to a telnet server on the specified port and then pass the provided command line argument to it.
	Accepts a port number and a command line argument.
	
	By Val
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
 
int main(int argc, char *argv[])
{
	int cliSocket;
	int commandLen;
	int bufferSize;
	int len = 0;
	int portNum;
	char fullCommand[4096];
	char receivedLine[128];
	char sentLine[128];
	struct sockaddr_in cliConfig;
	
	//Check that the number of arguments is correct and that the first argument can fit as an INT
	if (argc >= 3) {
		valueCheck = strtol(argv[1], &pointer, 10);
		if (endPtr == argv[1]) {
			fprintf(stderr, "First argument must be an integer!\n");
			return 0;
		}
		else if (*endPtr != '\0') {
			fprintf(stderr, "First argument must be an integer!\n");
			return 0;
		}
		else if (valueCheck < INT_MIN || valueCheck > INT_MAX) {
			fprintf(stderr, "Argument value out of range!\n");
			return 0;
		}
		portNum = atoi(argv[1]);
	}
	else if (argc <= 2) {
		fprintf(stderr, "Wrong number of arguments!\n");
		return 0;
	}

	bzero(sentLine, 128);
	bzero(receivedLine, 128);
	bzero(fullCommand, 4096);
	strncpy(fullCommand, argv[2], 4096);
	if (argc > 3) {
		for (int i = 3; i < argc; i++) {
			strcat(fullCommand, " ");
			strcat(fullCommand, argv[i]);
		}
	}
 
	cliSocket = socket(AF_INET,SOCK_STREAM, 0);
	memset(&cliConfig, 0, sizeof(struct sockaddr_in));
 
	cliConfig.sin_addr.s_addr = inet_addr("127.0.0.1");
	cliConfig.sin_family = AF_INET;
	cliConfig.sin_port = htons(PORTNUM);
 
	connect(cliSocket, (struct sockaddr *) &cliConfig, sizeof(cliConfig));
	puts("---Connection Created---");
 
	commandLen = strlen(fullCommand);

	send(cliSocket, fullCommand, commandLen, 0); //sending command
	recv(cliSocket, &len, sizeof(len), 0);
	recv(cliSocket, receivedLine, len, 0);
	printf("%s\n", receivedLine); 
	fgets(sentLine, 128, stdin); //getting username
	send(cliSocket, sentLine, strlen(sentLine) + 1, 0);
	recv(cliSocket, &len, sizeof(len), 0);
	recv(cliSocket, receivedLine, len, 0);
	printf("%s\n", receivedLine);
	fgets(sentLine, 128, stdin); //getting password
	send(cliSocket, sentLine, strlen(sentLine) + 1, 0);
	
	while (1) {
		bzero(receivedLine, 128);
		bufferSize = recv(cliSocket, &len, sizeof(len), 0);
		if (bufferSize == -1) {
			puts("---Something Went Wrong---");
			break;
		}
		if (bufferSize == 0) {
			puts("---Connection Closed---");
			break;
		}
        recv(cliSocket, receivedLine, len, 0);
		printf("%s\n", receivedLine);
	}
}