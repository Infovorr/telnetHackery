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

#define PORTNUM 8559
#define MAX_CLIENTS 1

int boundSocket;
int clientSocket;
int clientSize;
int lenA;
int lenB = 0;
int servSize;
int servSocket;
struct sockaddr_in clientConfig;
struct sockaddr_in servConfig;
char username[30];
char password[30];

int setup() {
	servSocket = socket(AF_INET, SOCK_STREAM, 0);
	memset(&servConfig, 0, sizeof(struct sockaddr_in));
	servConfig.sin_family = AF_INET;
	servConfig.sin_addr.s_addr = INADDR_ANY;
	servConfig.sin_port = htons(PORTNUM);
	servSize = sizeof(servConfig);
	boundSocket = bind(servSocket, (struct sockaddr *) &servConfig, servSize);
}

int validate(char username[], char password[]) {
	return 1;
}

int main(int argc, char *argv[]) {
	lenA = strlen("Enter a username:");
	FILE *filePointer;
	int pipeStatus;
	char fullCommand[10];
	char inputBuffer[4096];
	char pipeOutput[4096];
	
	setup();
	listen(servSocket, MAX_CLIENTS);
	clientSize = sizeof(struct sockaddr_in);
	clientSocket = accept(servSocket, (struct sockaddr *) &clientConfig, (socklen_t *) &clientSize);
	recv(clientSocket, inputBuffer, 4096, 0);
	strcpy(fullCommand, inputBuffer);
	printf("The size of the buffer is %d and the size of the command is %d\n", strlen(inputBuffer), strlen(fullCommand));
	send(clientSocket, &lenA, sizeof(lenA), 0);
	send(clientSocket, "Enter a username:", lenA, 0);
	recv(clientSocket, username, sizeof(username), 0);
	printf("Read username %s\n", username);
	send(clientSocket, &lenA, sizeof(lenA), 0);
	send(clientSocket, "Enter a password:", lenA, 0);
	recv(clientSocket, password, sizeof(password), 0);
	printf("Read password %s\n", password);
	if (validate(username, password)) {
		filePointer = popen(fullCommand, "r");
		puts("Executing command");
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
	sleep(10);
}
