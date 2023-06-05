#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>

#define MAX_SERVERS 3
#define MAX_STRLEN 256

int main() {
    struct Server {
        const char *address;
        int port;
    };
    struct Server servers[MAX_SERVERS] = {
        {"localhost", 5001},
        {"localhost", 5002},
        {"localhost", 5003}
    };

    int sockets[MAX_SERVERS];

    for (int i = 0; i < MAX_SERVERS; i++) {
        struct Server *server = &servers[i];

        if ((sockets[i] = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            printf("Could not create socket for Server %d\n", i + 1);
            exit(1);
        }

        struct sockaddr_in srv_addr;
        memset(&srv_addr, 0, sizeof(srv_addr));
        srv_addr.sin_family = AF_INET;
        srv_addr.sin_port = htons(server->port);
        inet_pton(AF_INET, server->address, &srv_addr.sin_addr);
    }

    char response[MAX_SERVERS][MAX_STRLEN];

    while (1) {
        char command[MAX_STRLEN];
        fgets(command, sizeof(command), stdin);

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "exit") == 0) {
            break;
        }

        for (int i = 0; i < MAX_SERVERS; i++) {
            struct Server *server = &servers[i];
            struct sockaddr_in srv_addr;
            memset(&srv_addr, 0, sizeof(srv_addr));
            srv_addr.sin_family = AF_INET;
            srv_addr.sin_port = htons(server->port);
            inet_pton(AF_INET, server->address, &srv_addr.sin_addr);

            if (sendto(sockets[i], command, strlen(command), 0, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0) {
                printf("Failed to send request to Server %d\n", i + 1);
                exit(1);
            }
        }

        memset(response, 0, sizeof(response));

        for (int i = 0; i < MAX_SERVERS; i++) {
            struct sockaddr_in srv_addr;
            socklen_t addr_len = sizeof(srv_addr);
            memset(&srv_addr, 0, sizeof(srv_addr));

            ssize_t num_bytes = recvfrom(sockets[i], response[i], sizeof(response[i]), 0, (struct sockaddr *)&srv_addr, &addr_len);
            if (num_bytes < 0) {
                printf("Error in receiving response from Server %d\n", i + 1);
                continue;
            }

            printf("%s\n", response[i]);
        }

        int same_value_count = 0;
        for (int i = 0; i < MAX_SERVERS; i++) {
            int match_count = 0;
            for (int j = 0; j < MAX_SERVERS; j++) {
                if (i != j && strcmp(response[i], response[j]) == 0) {
                    match_count++;
                }
            }
            if (match_count >= 1) {
                same_value_count++;
            }
        }

        if (same_value_count >= 1) {
            printf("TRUE\n");
        } else {
            printf("FALSE\n");
        }
    }

    for (int i = 0; i < MAX_SERVERS; i++) {
        close(sockets[i]);
    }

    return 0;
}
