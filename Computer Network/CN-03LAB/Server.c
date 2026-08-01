#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // 1. Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Define address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    // 3. Bind socket
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // 4. Listen for connections
    listen(server_fd, 3);

    printf("Server waiting on port %d...\n", PORT);

    // 5. Accept connection
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

    char msg[100];

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));

        int n = read(new_socket, buffer, sizeof(buffer) - 1);

        if (n <= 0)
            break;

        buffer[n] = '\0';

        

        if (buffer[0] == 'b' && buffer[1] == 'y' && buffer[2] == 'e' ){
            printf("Thank u for using our app..");
            break;
        }
        printf("Client: %s\n", buffer);

        printf("Server: ");
        gets ( msg );
        

        send(new_socket, msg, strlen(msg), 0);

        if (msg[0] == 'b' && msg[1] == 'y' && msg[2] == 'e'){
            printf("Thank u for using our app..");
            break;
        }
    }

    close(new_socket);
    close(server_fd);

    return 0;
}