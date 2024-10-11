#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void handleHEAD(char* message) {
    sprintf(message, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
}

void handleGET(char* file_name, char* message) {
    memset(message, 0, 4096); // Clear the message buffer
    handleHEAD(message);

    FILE* file_ptr = fopen(file_name, "r");
    if (file_ptr == NULL) {
        sprintf(message, "HTTP/1.1 404 Not Found\r\n\
			Content-Type: text/html\r\nContent-Length: 37\r\n\r\n\
			<html><body><h1>404 Not Found</h1></body></html>");
        return;
    }

    fseek(file_ptr, 0, SEEK_END);
    int file_size = ftell(file_ptr);
    rewind(file_ptr);

    char* file_contents = malloc(file_size + 1);
    fread(file_contents, 1, file_size, file_ptr);
    file_contents[file_size] = '\0'; // Null-terminate file contents
    fclose(file_ptr);
    strcat(message, file_contents); // Append file contents

    free(file_contents); 
}

void handleMessage(char* message, int client_socket) {
    char* dup_message = strdup(message);
    char* token_array[3];

    char* token = strtok(dup_message, " ");
    token_array[0] = token; // Method
    token = strtok(NULL, " ");
    token_array[1] = token; // Resource
    token = strtok(NULL, " ");
    token_array[2] = token; // HTTP version

    char* file_name = malloc(strlen(token_array[1]) + 1);
    strcpy(file_name, token_array[1] + 1); // Skip leading '/'

    if (strcmp(token_array[0], "GET") == 0) {
        handleGET(file_name, message);
    } else if (strcmp(token_array[0], "HEAD") == 0) {
        handleHEAD(message);
    } else {
        sprintf(message, "HTTP/1.1 501 Not Implemented\r\n\r\n");
    }

    send(client_socket, message, strlen(message), 0);
    free(file_name);
    free(dup_message);
    close(client_socket);
}

void handleConnection(){
    int connection = socket(AF_INET, SOCK_STREAM, 0);
    if(connection == -1){
	perror("server connection error.\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address; 
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);

    setsockopt(connection, SOL_SOCKET, SO_REUSEPORT, (char *)&connection, sizeof connection);

    int bind_status = bind(connection, (struct sockaddr*)&address, sizeof(address));
    if(bind_status == -1){
	perror("server bind error. \n");
	exit(EXIT_FAILURE);
    }

    if(listen(connection, 1) == -1){
        perror("Listen error.");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for connections...\n");

    while(1){
	struct sockaddr_in client_address;
	socklen_t client_address_len = sizeof(client_address);

	int client_socket = accept(connection, (struct sockaddr*)&client_address, &client_address_len);
	if(client_socket < 0){
	    perror("Accept error\n");
	    continue;
	}

	char buff[4096] = {0};
	recv(client_socket, buff, sizeof(buff), 0);

	handleMessage(buff,client_socket);
	close(client_socket);
    }

    close(connection);
}

int main(void){
    handleConnection();
    return 0;
}
