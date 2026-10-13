#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char reg_no[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    char name[BUFFER_SIZE];
    char branch[BUFFER_SIZE];
    char college[BUFFER_SIZE];

    // 1. Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Define server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // inet_pton(AF_INET, "10.158.201.130", &serv_addr.sin_addr);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // 3. Connect to server
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    while (1)
    {

        printf("\nEnter Registration  Number ( or type 'bye' to exit ) : ");

        fgets(reg_no, BUFFER_SIZE, stdin);
        reg_no[strcspn(reg_no, "\n ")] = 0;

        send(sock, reg_no, strlen(reg_no), 0);

        if (strcmp(reg_no, " bye ") == 0)
        {
            printf("Thank u for using our app..");
            break;
        }

        memset(buffer, 0, sizeof(buffer));
        read(sock, buffer, sizeof(buffer));

        if (strcmp(reg_no, " bye ") == 0)
        {
            printf("Thank u for using our app..");
            break;
        }
        printf("Server Response : %s \n", buffer);

        if (strcmp(buffer, "Data not found") == 0)
        {
            printf("\nEnter Registration  Number ( or type 'bye' to exit ) : ");

            fgets(name, BUFFER_SIZE, stdin);
            reg_no[strcspn(name, "\n ")] = 0;
            send(sock, name, strlen(name), 0);

            fgets(branch, BUFFER_SIZE, stdin);
            reg_no[strcspn(branch, "\n ")] = 0;
            send(sock, branch, strlen(branch), 0);

            fgets(college, BUFFER_SIZE, stdin);
            reg_no[strcspn(college, "\n ")] = 0;
            send(sock, college, strlen(college), 0);


        }
    }
    printf("exit...");

    close(sock);

    return 0;
}