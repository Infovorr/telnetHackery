/*
	Telnet Server
	
	Creates a server that waits for client connections, attempts to verify them, 
	and then executes a single command before returning the output to the client.
	
	Has the option of a single argument: an integer representing the port number 
	the server will use. Default port number is 100.
	
	By Val
*/

#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>
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

//Deprecated function for dubiously entertaining times
int mysteryFunction() {
	FILE *mysFile;
	mysFile = fopen("mystery.txt","w");
	if (mysFile == NULL) {
		puts("Could not open file.");
		exit(1);
	}
	fputs("Mystery function activated!", mysFile);
	fclose(mysFile);
	return 0;
}

//Sets up the server's socket
int setup(int portNum) {
	servSocket = socket(AF_INET, SOCK_STREAM, 0);
	memset(&servConfig, 0, sizeof(struct sockaddr_in));
	servConfig.sin_family = AF_INET;
	servConfig.sin_addr.s_addr = INADDR_ANY;
	servConfig.sin_port = htons(portNum);
	servSize = sizeof(servConfig);
	boundSocket = bind(servSocket, (struct sockaddr *) &servConfig, servSize);
	printf("Bound socket at port %d\n", portNum);
	return 0;
}

//Reads from a file containing usernames and passwords, then compares to the provided username and password.
//By default there exists a single username and password, test and test (respectively).
//Ideally this file should be encrypted but, well, think of it this way: now you've got something else to
//try to hack into!
int validate(char username[], char password[]) {
	FILE *passFile;
	char line[256];
	char *pass;
	char *passwyrd;
	char *user;
	char *usyr;
	const char breaker[2] = ",";
	int found[] = {0,0};
	int success = 0;

	usyr = strtok(username, "\n");
	passwyrd = strtok(password, "\n");
	
	passFile = fopen("pass.csv", "r");

	if (passFile == NULL) {
		puts("Could not open file.");
		exit(1);
	}
	
	while (fgets(line, sizeof(line), passFile) != NULL) {
		user = strtok(line, breaker);
		pass = strtok(NULL, breaker);
		if (*user == *usyr && *pass == *passwyrd) {
			success = 1;
			break;
		}
	}
	fclose(passFile);
	
	return success;
}

int main(int argc, char *argv[]) {
	lenA = strlen("Enter a username:");
	FILE *filePointer;
	int commandSize = 0;
	int pipeStatus;
	long valueCheck;
	char fullCommand[60];
	char inputBuffer[4096];
	char pipeOutput[4096];
	char *pointer;
	
	//Check that the number of arguments is correct and that the first argument can fit as an INT
	if (argc == 2) {
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
		setup(atoi(argv[1]));
	}
	else if (argc > 2) {
		fprintf(stderr, "Wrong number of arguments!\n");
		return 0;
	}
	else {
		setup(100);
	}

	puts("Listening for connection...\n");
	listen(servSocket, MAX_CLIENTS);
	clientSize = sizeof(struct sockaddr_in);
	clientSocket = accept(servSocket, (struct sockaddr *) &clientConfig, (socklen_t *) &clientSize);
	puts("Accepted connection.\n");
	recv(clientSocket, &commandSize, sizeof(commandSize), 0);
	recv(clientSocket, inputBuffer, commandSize, 0);
	strcpy(fullCommand, inputBuffer);
	printf("%p\n", &fullCommand);
	send(clientSocket, &lenA, sizeof(lenA), 0);
	send(clientSocket, "Enter a username:", lenA, 0);
	recv(clientSocket, username, sizeof(username), 0);
	send(clientSocket, &lenA, sizeof(lenA), 0);
	send(clientSocket, "Enter a password:", lenA, 0);
	recv(clientSocket, password, sizeof(password), 0);
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
}
