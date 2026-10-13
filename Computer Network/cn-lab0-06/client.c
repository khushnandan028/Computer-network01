#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 4096

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
               &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(1);
    }

    if (listen(server_fd, 10) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(1);
    }

    signal(SIGCHLD, SIG_IGN);

    printf("Server is running on port %d...\n", PORT);

    while (1)
    {
        client_fd = accept(server_fd, NULL, NULL);

        if (client_fd < 0)
        {
            perror("Accept failed");
            continue;
        }

        pid_t pid = fork();

        if (pid < 0)
        {
            perror("Fork failed");
            close(client_fd);
            continue;
        }

        if (pid == 0)
        {
            // Child process handles one client
            close(server_fd);

            char buffer[BUFFER_SIZE];
            char response[256];

            size_t total = 0;

            while (total < sizeof(buffer) - 1)
            {
                ssize_t n = read(client_fd,
                                 buffer + total,
                                 sizeof(buffer) - 1 - total);

                if (n == 0)
                    break;

                if (n < 0)
                {
                    perror("Read failed");
                    close(client_fd);
                    exit(1);
                }

                total += n;
            }

            buffer[total] = '\0';

            // Request format: StudentID followed by newline
            // and then the student records.
            char *newline = strchr(buffer, '\n');

            if (newline == NULL)
            {
                snprintf(response, sizeof(response),
                         "Invalid Request");
            }
            else
            {
                *newline = '\0';

                char *student_id = buffer;
                char *records = newline + 1;
                char *saveptr;
                char *record;
                int found = 0;

                record = strtok_r(records, "\n", &saveptr);

                while (record != NULL)
                {
                    char id[32];
                    char name[64];
                    char branch[64];
                    char extra;

                    if (sscanf(record, "%31s %63s %63s %c",
                               id, name, branch, &extra) == 3
                        && strcmp(id, student_id) == 0)
                    {
                        snprintf(response, sizeof(response),
                                 "Record Found: %s", record);
                        found = 1;
                        break;
                    }

                    record = strtok_r(NULL, "\n", &saveptr);
                }

                if (!found)
                {
                    snprintf(response, sizeof(response),
                             "Record Not Found");
                }
            }

            // Send response to client
            size_t len = strlen(response);
            size_t sent = 0;

            while (sent < len)
            {
                ssize_t n = write(client_fd,
                                  response + sent,
                                  len - sent);

                if (n <= 0)
                    break;

                sent += n;
            }

            printf("Child process %d handled a client.\n",
                   getpid());

            close(client_fd);
            exit(0);
        }
        else
        {
            // Parent continues accepting new clients
            close(client_fd);
        }
    }

    close(server_fd);
    return 0;
}