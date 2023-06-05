#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PAIRS 10000
#define MAX_STRLEN 256

typedef struct {
    char key[MAX_STRLEN];
    char value[MAX_STRLEN];
} KeyValuePair;

KeyValuePair kv_store[MAX_PAIRS];
int kv_count = 0;

void put(const char *key, const char *value) {
    int i;
    for (i = 0; i < kv_count; i++) {
        if (strcmp(kv_store[i].key, key) == 0) {
            strcpy(kv_store[i].value, value);
            return;
        }
    }

    if (kv_count < MAX_PAIRS) {
        strcpy(kv_store[kv_count].key, key);
        strcpy(kv_store[kv_count].value, value);
        kv_count++;
    }
}

const char *get(const char *key) {
    int i;
    for (i = 0; i < kv_count; i++) {
        if (strcmp(kv_store[i].key, key) == 0) {
            return kv_store[i].value;
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Input: %s port number\n", argv[0]);
        return 1;
    }

    const int SERVER_PORT = atoi(argv[1]);

    struct sockaddr_in srv_addr;
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(SERVER_PORT);
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int sock;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Could not create listen socket\n");
        exit(1);
    }

    if ((bind(sock, (struct sockaddr *)&srv_addr, sizeof(srv_addr))) < 0) {
        printf("Could not bind socket\n");
        exit(1);
    }

    struct sockaddr_in cli_addr;
    int cli_addr_len = sizeof(cli_addr);

    int maxlen = 1024;
    int n = 0;
    char RecvBuffer[maxlen];
    char SendBuffer[maxlen];

    srand(time(NULL));

    while (1) {
        n = recvfrom(sock, &RecvBuffer, sizeof(RecvBuffer), 0, (struct sockaddr *)&cli_addr, &cli_addr_len);
        if (n > 0) {
            RecvBuffer[n] = '\0';

            char *command = strtok(RecvBuffer, " \n");
            char *key = strtok(NULL, " \n");

            if (command != NULL && key != NULL && strcmp(command, "get") == 0) {
                char response[MAX_STRLEN];
                memset(response, 0, sizeof(response));

                if (SERVER_PORT == 5001) {
                    strcpy(response, "1");
                } else {
                    int random_value = rand() % 3 + 1;
                    sprintf(response, "%d", random_value);
                }

                printf("%s returned\n", response);

                sendto(sock, response, strlen(response), 0, (struct sockaddr *)&cli_addr, cli_addr_len);
            }
        }
    }
    close(sock);

    return 0;
}
