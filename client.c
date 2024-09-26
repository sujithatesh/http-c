// #define DEBUG

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int handleConnection(struct sockaddr_in address, int client_connection, char* message){
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);


    if(connect(client_connection, (struct sockaddr *)&address, sizeof(address)) == -1){
	perror("Connection failure");
	close(client_connection);
	return EXIT_FAILURE;
    }


    send(client_connection, message, strlen(message), 0);
    memset(message, 0, sizeof(*message));

    size_t bytes_received = recv(client_connection, message, 4096, 0);
    if(bytes_received < 0){
	perror("No bytes received");
	close(client_connection);
	return EXIT_FAILURE;
    }

    message[bytes_received] = '\0';
    printf("HTTP/1.0 200 OK\n\r%s\n", message);


#ifdef DEBUG
    printf("Client said: %s\n", message);
#endif
    return 0;
}

int main(void){
    int client_connection = socket(AF_INET, SOCK_STREAM, 0);
    if(!client_connection){
	printf("client socket connection error.\n");
    }
    struct sockaddr_in address; 

    char message[4096] = "Hello World!\0";
    printf(">>> ");
    scanf("%[^\n]",message);

    handleConnection(address, client_connection, message);

    return 0;
}

