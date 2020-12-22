/*
	Telnet Client
	
	Attempts to connect to a telnet server on the specified port and then pass the 
	provided command line argument to it.
	Accepts a port number and a command line argument.
	
	By Val
*/

#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
	int bufferSize;
	int cliSocket;
	int commandSize;
	int len = 0;
	int portNum;
	int valueCheck;
	char fullCommand[4096];
	char inputLine[100];
	char outputLine[100];
	char *pointer;
	struct sockaddr_in cliConfig;
	
	//Check that the number of arguments is correct and that the first argument can fit as an INT
	if (argc >= 3) {
		valueCheck = strtol(argv[1], &pointer, 10);
		if (pointer == argv[1]) {
			fprintf(stderr, "First argument must be an integer!\n");
			return 0;
		}
		else if (*pointer != '\0') {
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

	bzero(fullCommand, 4096);
	
	//Convert the provided command line argument into a string
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
	cliConfig.sin_port = htons(portNum);
	
	connect(cliSocket, (struct sockaddr *) &cliConfig, sizeof(cliConfig));
	puts("Connection established!");
	
	bzero(outputLine, 100);
	bzero(inputLine, 100);
	
	commandSize = strlen(fullCommand);
	
	send(cliSocket, &commandSize, sizeof(commandSize), 0);
	send(cliSocket, fullCommand, commandSize, 0); //sending command
	recv(cliSocket, &len, sizeof(len), 0);
	recv(cliSocket, inputLine, len, 0);
	printf("%s\n", inputLine);
	fgets(outputLine, 100, stdin); //getting username
	send(cliSocket, outputLine, strlen(outputLine), 0);
	recv(cliSocket, &len, sizeof(len), 0);
	recv(cliSocket, inputLine, len, 0);
	printf("%s\n", inputLine);
	fgets(outputLine, 100, stdin); //getting password
	send(cliSocket, outputLine, strlen(outputLine), 0);
	
	while (1) {
		bzero(inputLine, 100);
		bufferSize = recv(cliSocket, &len, sizeof(len), 0);
		if (bufferSize == -1) {
			puts("---Unexpected Connection Failure---");
			break;
		}
		if (bufferSize == 0) {
			puts("---Connection Closed---");
			break;
		}
		recv(cliSocket, inputLine, len, 0);
		printf("%s\n", inputLine);
	}
}
