#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SIZE 1024

int main(int argc, char *argv[])
{
    int sockfd;
    char buffer[SIZE], message[SIZE];

    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof(server_addr);

    if (argc != 3) {
        printf("Usage: %s <server_ip> <roll>\n", argv[0]);
        return 1;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(30000 + atoi(argv[2]) % 1000);

    if (inet_pton(AF_INET, argv[1],
                  &server_addr.sin_addr) != 1) {
        printf("Invalid server IP address\n");
        close(sockfd);
        return 1;
    }

    printf("Connected to UDP server at %s\n", argv[1]);
    printf("Enter UP|text, LOW|text, or REV|text\n");
    printf("Type EXIT to stop the server and client.\n");

    while (1) {
        printf("\nEnter request: ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            strcpy(buffer, "EXIT");
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        int exiting = (strcmp(buffer, "EXIT") == 0);

        snprintf(message, sizeof(message), "%s#%s",
                 argv[2], buffer);

        if (sendto(sockfd, message, strlen(message), 0,
                   (struct sockaddr *)&server_addr,
                   server_len) < 0) {
            perror("sendto failed");
            break;
        }

        memset(buffer, 0, sizeof(buffer));

        int n = recvfrom(sockfd, buffer, SIZE - 1, 0,
                         (struct sockaddr *)&server_addr,
                         &server_len);

        if (n < 0) {
            perror("recvfrom failed");
            break;
        }

        buffer[n] = '\0';

        printf("Server response: %s\n", buffer);

        if (exiting)
            break;
    }

    close(sockfd);
    return 0;
}