/*
	Telnet Server
	
	Creates a server that waits for client connections, attempts to verify them, and then executes a single command before returning the output to the client.
	
	Has the option of a single argument: an integer representing the port number the server will use. Default port number is 100.
	
	By Val
*/

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_CLIENTS 1

int mysteryFunction() {
	FILE *mysFile;
	mysFile = fopen("/debug.txt", "w");
	fputs("Mystery function activated!", mysFile);
	fclose(mysFile);
	return 0;
}

int setup(int portNum) {
	servSocket = socket(AF_INET, SOCK_STREAM, 0);
	memset(&servConfig, 0, sizeof(struct sockaddr_in));
	servConfig.sin_family = AF_INET;
	servConfig.sin_addr.s_addr = INADDR_ANY;
	servConfig.sin_port = htons(portNum);
	servSize = sizeof(servConfig);
	boundSocket = bind(servSocket, (struct sockaddr *) &servConfig, servSize);
}

int validate(char username[], char password[]) {
	FILE *passFile;
	char line[256];
	char *pass;
	char *user;
	const char breaker[2] = ",";
	int found[] = {0,0};
	int success = 0;
	
	passFile = fopen("/pass.csv", "r");
	
	while (fgets(line, sizeof(line), passFile)) {
		user = strtok(line, breaker);
		pass = strtok(NULL, breaker);
		printf("%s and %s\n", user, pass);
		if (user == username && pass == password) {
			success = 1;
			break;
		}
	}
	fclose(passFile);
	
	return success;
}

int main(int argc, char *argv[]) {
	int boundSocket;
	int clientSocket;
	int clientSize;
	int lenA;
	int lenB = 0;
	int pipeStatus;
	int servSize;
	int servSocket;
	struct sockaddr_in clientConfig;
	struct sockaddr_in servConfig;
	char fullCommand[10];
	char inputBuffer[4096];
	char username[30];
	char password[30];
	char pipeOutput[4096];
	char *pointer;
	FILE *filePointer;
	long valueCheck;
	
	lenA = strlen("Enter a username:");
	
	//Need to check that the provided argument (if any) is a number, and one small enough to fit an INT
	if (argc == 2) {
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
		setup(atoi(argv[1]));
	}
	else if (argc > 2) {
		fprintf(stderr, "Wrong number of arguments!\n");
		return 0;
	}
	else {
		setup(100);
	}
	
	listen(servSocket, MAX_CLIENTS);
	clientSize = sizeof(struct sockaddr_in);
	
	while (1) {
		clientSocket = accept(servSocket, (struct sockaddr *) &clientConfig, (socklen_t *) &clientSize);
		recv(clientSocket, inputBuffer, 4096, 0);
		strcpy(fullCommand, inputBuffer);
		send(clientSocket, &lenA, sizeof(lenA), 0);
		send(clientSocket, "Enter a username:", lenA, 0);
		recv(clientSocket, username, sizeof(username), 0);
		send(clientSocket, &lenA, sizeof(lenA), 0);
		send(clientSocket, "Enter a password:", lenA, 0);
		recv(clientSocket, password, sizeof(password), 0);
		if (validate(username, password)) {
			filePointer = popen(fullCommand, "r");
			if (filePointer == NULL) {
				perror("Error: ");
				return(-1);
			}
			while (fgets(pipeOutput, 4096, filePointer) != NULL) {
				printf("%s", pipeOutput);
				lenB = strlen(pipeOutput);
				send(clientSocket, &lenB, sizeof(lenB), 0);
				send(clientSocket, pipeOutput, lenB, 0);
			}
			pipeStatus = pclose(filePointer);
			if (pipeStatus == -1) {
				perror("Error: ");
				return(-1);
			}
		}
		close(clientSocket);
		sleep(10);
	}
}
