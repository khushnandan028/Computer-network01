// Roll: 24105003
// Service Type: A
// d = 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define ROLL "24105003"
#define PORT (30000 + 3)
#define SIZE 1024

void process_request(char *request, char *response)
{
    char *command = strtok(request, "|");
    char *text = strtok(NULL, "");

    if (command == NULL || text == NULL) {
        strcpy(response, "ERROR: Invalid request");
        return;
    }

    if (strcmp(command, "UP") == 0) {
        for (int i = 0; text[i]; i++)
            text[i] = toupper((unsigned char)text[i]);

        strcpy(response, text);
    }
    else if (strcmp(command, "LOW") == 0) {
        for (int i = 0; text[i]; i++)
            text[i] = tolower((unsigned char)text[i]);

        strcpy(response, text);
    }
    else if (strcmp(command, "REV") == 0) {
        int len = strlen(text);

        for (int i = 0; i < len; i++)
            response[i] = text[len - 1 - i];

        response[len] = '\0';
    }
    else {
        strcpy(response, "ERROR: Invalid command");
    }
}

int main()
{
    int sockfd;
    char buffer[SIZE], response[SIZE];

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        return 1;
    }

    printf("Server running on port %d\n", PORT);

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        memset(response, 0, sizeof(response));

        int n = recvfrom(sockfd, buffer, SIZE - 1, 0,
                         (struct sockaddr *)&client_addr,
                         &client_len);

        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }

        buffer[n] = '\0';

        printf("\nClient IP: %s, Port: %d\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));

        char *prefix = strchr(buffer, '#');

        if (prefix == NULL) {
            strcpy(response, "ERROR: Unauthorized client");
        }
        else {
            *prefix = '\0';

            if (strcmp(buffer, ROLL) != 0) {
                strcpy(response, "ERROR: Unauthorized client");
            }
            else {
                char *request = prefix + 1;

                printf("Received message: %s\n", request);

                if (strcmp(request, "EXIT") == 0) {
                    strcpy(response, "Server shutting down");
                    sendto(sockfd, response, strlen(response), 0,
                           (struct sockaddr *)&client_addr,
                           client_len);

                    printf("Sent response: %s\n", response);
                    printf("Server exiting...\n");

                    break;
                }

                process_request(request, response);
            }
        }

        sendto(sockfd, response, strlen(response), 0,
               (struct sockaddr *)&client_addr, client_len);

        printf("Sent response: %s\n", response);
    }

    close(sockfd);
    return 0;
}