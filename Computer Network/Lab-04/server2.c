#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void find_student(char *reg_no, char *response)
{
    FILE *fp = fopen("student.txt", "r");
    // fprintf(fp, "%d\n,"num")
    if (fp == NULL)
    {
        strcpy(response, " Error opening file ");
        return;
    }
    char file_reg[50], name[50], branch[50], college[100];
    int found = 0;
    while (fscanf(fp, " %s %s %s %s ", file_reg, name, branch, college) != EOF)
    {
        if (strcmp(file_reg, reg_no) == 0)
        {
            sprintf(response, " Name : %s , Branch : %s , College : %s ", name, branch, college);
            found = 1;
            break;
        }
    }
    if (!found)
    {
        strcpy(response, "Data not found");
    }
    fclose(fp);
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    char name[BUFFER_SIZE];
    char branch[BUFFER_SIZE];
    char college[BUFFER_SIZE];
    char reg_no[BUFFER_SIZE];

    // 1. Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // // 2. Define address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    // // 3. Bind socket
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // // 4. Listen for connections
    listen(server_fd, 3);

    printf("Server waiting on port %d...\n", PORT);

    // // 5. Accept connection
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

    char msg[100];

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));

        int n = read(new_socket, buffer, sizeof(buffer) - 1);
        if (n <= 0)
            break;

        buffer[n] = '\0';
        printf("Client: %s\n", buffer);

        printf("Server: ");
        find_student(buffer, response);
        printf("Regi. No. : %s\n", buffer);
        printf("response: %s\n", response);

        send(new_socket, response, strlen(response), 0);

        if (strcmp(response, "Data not found") == 0) {

            printf("in Data not found");
            // Registration no..
            printf("Client Reg. no.: %s\n", buffer);



            // Name...
            memset(name, 0, sizeof(name));

            int o = read(new_socket, name, sizeof(name) - 1);
            if (0 <= 0)
                break;

            name[0] = '\0';
            printf("Client Name: %s\n", name);


            // Branch...
            memset(branch, 0, sizeof(branch));

            int p = read(new_socket, branch, sizeof(branch) - 1);
            if (p <= 0)
                break;

            branch[p] = '\0';
            printf("Client Branch: %s\n", branch);

             // Branch...
            memset(college, 0, sizeof(college));

            int q = read(new_socket, college, sizeof(college) - 1);
            if (q)
                break;

            college[q] = '\0';
            printf("Client College: %s\n", college);
        }
    }

    close(new_socket);
    close(server_fd);

    return 0;
}