#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/select.h>
#include <arpa/inet.h>

#define PORT 5000
#define MAXLINE 1024
#define MAX_CLIENTS 10

void saveClientDetails(const char* string, const char* clientIP, int clientPort) {
    FILE* file = fopen("client_details.txt", "a");
    if (file == NULL) {
        perror("File opening failed");
        return;
    }

    fprintf(file, "Base String: %s\n", string);
    fprintf(file, "Client IP: %s\n", clientIP);
    fprintf(file, "Client Port: %d\n\n", clientPort);

    fclose(file);
}

int main() {
    int sockfd, connfd, nready;
    char buffer[MAXLINE];
    fd_set allset, rset;
    ssize_t n;
    socklen_t len;
    struct sockaddr_in cliaddr, servaddr;
    int client_fds[MAX_CLIENTS];

    // Create listening TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
 
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Bind the server address to the listening socket
    bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    listen(sockfd, 10);

    int maxfd = sockfd;
    FD_ZERO(&allset);
    FD_SET(sockfd, &allset);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_fds[i] = -1;
    }

    for (;;) {
        rset = allset;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &rset)) {
            len = sizeof(cliaddr);
            connfd = accept(sockfd, (struct sockaddr*)&cliaddr, &len);

            // Find an empty slot for the new client
            int i;
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (client_fds[i] == -1) {
                    client_fds[i] = connfd;
                    break;
                }
            }

            if (i == MAX_CLIENTS) {
                // Reached maximum number of clients
                write(connfd, "Server is busy. Try again later.\n", strlen("Server is busy. Try again later.\n"));
                close(connfd);
            } else {
                char clientIP[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(cliaddr.sin_addr), clientIP, INET_ADDRSTRLEN);
                int clientPort = ntohs(cliaddr.sin_port);

                printf("New client connected\n");
                printf("Client IP: %s\n", clientIP);
                printf("Client Port: %d\n", clientPort);

                FD_SET(connfd, &allset);
                maxfd = (connfd > maxfd) ? connfd : maxfd;

                // Save the client details to a file
                saveClientDetails("LIF20/21BSSE0XXX", clientIP, clientPort);
            }

            if (--nready <= 0)
                continue;
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            connfd = client_fds[i];
            if (connfd != -1 && FD_ISSET(connfd, &rset)) {
                bzero(buffer, sizeof(buffer));
                n = read(connfd, buffer, sizeof(buffer));

                if (n == 0) {
                    // Client closed the connection
                    printf("Client disconnected\n");
                    close(connfd);
                    FD_CLR(connfd, &allset);
                    client_fds[i] = -1;
                } else {
                    // Generate the updated string
                    int clientPort = ntohs(cliaddr.sin_port);
                    sprintf(buffer + strlen(buffer), "%d", clientPort);
                    write(connfd, buffer, strlen(buffer));
                    printf("Client Message Received: %s\n", buffer);
                    printf("Client Port No: %d\n", clientPort);
                }
            }
        }
    }

    return 0;
}
