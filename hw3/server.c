#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_PAIRS 10000 // The maximum number of key-value pairs
#define MAX_STRLEN 256 // The maximum length of key and value strings

/* KVS structure */
typedef struct {
    char key[MAX_STRLEN];
    char value[MAX_STRLEN];
} KeyValuePair;

KeyValuePair kv_store[MAX_PAIRS];
int kv_count = 0;

// Function to put a key-value pair into the dictionary
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

// Function to get the value for a key from the dictionary
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
	if ( argc < 2 ){
	 printf("Input : %s port number\n", argv[0]);
	 return 1;
	}

	int SERVER_PORT = atoi(argv[1]);

    // 리더와 팔로워 구분
    bool isLeader = atoi(argv[1]);
    if(isLeader){
        //Leader
        while (1) {
            //리더 소켓
            int leader_sock;
            
            struct sockaddr_in leader_addr;
            memset(&leader_addr, 0, sizeof(leader_addr));
            leader_addr.sin_family = AF_INET;
            leader_addr.sin_port = htons(SERVER_PORT);
            leader_addr.sin_addr.s_addr = htonl(INADDR_ANY);
            
            if ((leader_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                printf("Could not create listen leader socket\n");
                exit(1);
            }

            struct sockaddr_in cli_addr;
            int cli_addr_len = sizeof(cli_addr);

            int  maxlen = 1024;
            int n = 0;
            
            char RecvBuffer[maxlen];
            char SendBuffer[maxlen];
            char FollowerBuffer[maxlen];
            

            if ((bind(leader_sock, (struct sockaddr *)&leader_addr, sizeof(leader_addr))) < 0) {
                printf("Could not bind leader socket\n");
                exit(1);
            }
            
              n = recvfrom(leader_sock, &RecvBuffer, sizeof(RecvBuffer), 0, (struct sockaddr *)&cli_addr, &cli_addr_len);
              if (n > 0) {
                  RecvBuffer[n]='\0';
                  printf("%s", RecvBuffer);
                  char cmd[10], key[MAX_STRLEN], value[MAX_STRLEN];
                  int cnt = sscanf(RecvBuffer, "%s %s %s", cmd, key, value);
                  
                  if(strncmp(cmd, "put", 3)==0){
                      char *p=strtok(RecvBuffer," ");
                      if(p==NULL) {
                          sprintf(SendBuffer, "Invalid command1");
                          printf("Invalid command1\n");
                      }else{
                          char *key = strtok(NULL, " ");
                          if(key==NULL){
                              sprintf(SendBuffer, "Invalid command2");
                              printf("Invalid command2\n");
                          }else{
                              char *value = strtok(NULL, " ");
                              if(value==NULL){
                                  sprintf(SendBuffer, "Invalid command3");
                                  printf("Invalid command3\n");
                              } else{
                                  char *trash = strtok(NULL, " ");
                                  if(trash != NULL){
                                      sprintf(SendBuffer, "Invalid command4");
                                      printf("Invalid command4\n");
                                  } else {
                                      put(key, value);
                                      sprintf(SendBuffer, "OK");
                                      sprintf(FollowerBuffer, "%s %s %s", cmd, key, value);
                                      printf("PUT is done\n");
                                      
                                      // 팔로워들한테 보내기 위한 소켓
                                      int follower_sock;
                                      
                                      struct sockaddr_in follower_addr;
                                      memset(&follower_addr, 0, sizeof(follower_addr));
                                      follower_addr.sin_family = AF_INET;
                                      follower_addr.sin_port = htons(SERVER_PORT+1);
                                      follower_addr.sin_addr.s_addr=htonl(INADDR_ANY);
                                      
                                      if((follower_sock = socket(AF_INET, SOCK_DGRAM, 0))<0){
                                          printf("Could not create follower socket\n");
                                          exit(1);
                                      }
                                      sendto(follower_sock, FollowerBuffer, strlen(FollowerBuffer), 0, (struct sockaddr *)&follower_addr, sizeof(follower_addr));
                                      close(follower_sock);
                                  }
                              }
                          }
                      }
                  } else if(strcmp(cmd, "get")==0 && strlen(key)>0 && cnt==2){
                      const char *result = get(key);
                      if(result==NULL) {
                          sprintf(SendBuffer, "Key not found");
                          printf("Key not found.\n");
                      }else {
                          char *newline = strchr(result, '\n');
                          if(newline){
                              *newline = '\0';
                          }
                          sprintf(SendBuffer, "%s", result);
                          printf("GET is done.\n");
                      }
                  }else if (strcmp(cmd, "put")==0 && strlen(key)>0 && strlen(value)>0 && cnt==3){
                      put(key, value);
                      sprintf(SendBuffer, "OK");
                      sprintf(FollowerBuffer, "%s %s %s", cmd, key, value);
                      printf("PUT is done\n");
                      
                      // 팔로워들한테 보내기 위한 소켓
                      int follower_sock;
                      
                      struct sockaddr_in follower_addr;
                      memset(&follower_addr, 0, sizeof(follower_addr));
                      follower_addr.sin_family = AF_INET;
                      follower_addr.sin_port = htons(SERVER_PORT+1);
                      follower_addr.sin_addr.s_addr=htonl(INADDR_ANY);
                      
                      if((follower_sock = socket(AF_INET, SOCK_DGRAM, 0))<0){
                          printf("Could not create follower socket\n");
                          exit(1);
                      }
                      // 팔로워로 전송
                      sendto(follower_sock, FollowerBuffer, strlen(FollowerBuffer), 0, (struct sockaddr *)&follower_addr, sizeof(follower_addr));
                      close(follower_sock);
                      
                  }else {
                      sprintf(SendBuffer, "Invalid command5");
                      printf("Invalid command5\n");
                  }
                  sendto(leader_sock, SendBuffer, strlen(SendBuffer), 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
              }
            close(leader_sock);
        }
    }else{
        //Follower
        int follower_sock;
        if ((follower_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            printf("Could not create follower socket\n");
            exit(1);
        }
        struct sockaddr_in follower_addr;
        memset(&follower_addr, 0, sizeof(follower_addr));
        follower_addr.sin_family = AF_INET;
        follower_addr.sin_port = htons(SERVER_PORT);
        follower_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(follower_sock, (struct sockaddr*)&follower_addr, sizeof(follower_addr)) < 0) {
            printf("Could not bind follower socket\n");
            exit(1);
        }

        socklen_t follower_addr_len = sizeof(follower_addr);
        if (getsockname(follower_sock, (struct sockaddr*)&follower_addr, &follower_addr_len) < 0) {
            printf("Could not get follower socket name\n");
            exit(1);
        }

        struct sockaddr_in leader_addr;
        memset(&leader_addr, 0, sizeof(leader_addr));
        leader_addr.sin_family = AF_INET;
        leader_addr.sin_port = htons(SERVER_PORT);
        leader_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        while(1) {
            char FollowerBuffer[MAX_STRLEN];
            
            socklen_t leader_addr_len = sizeof(leader_addr);
            int n = recvfrom(follower_sock, &FollowerBuffer, MAX_STRLEN, 0, (struct sockaddr *) &leader_addr, &leader_addr_len);
            if(n>0) {
                FollowerBuffer[n]='\0';
                printf("leader message: %s\n", FollowerBuffer);

                char cmd[10], key[MAX_STRLEN], value[MAX_STRLEN];
                int cnt = sscanf(FollowerBuffer, "%s %s %s", cmd, key, value);

                if(strcmp(cmd, "put")==0&&strlen(key)>0&&strlen(value)>0&&cnt==3){
                    put(key, value);
                    printf("PUT OK\n");
                    printf("put %s %s\n", key, value);
                }else{
                    printf("leader message: %s\n", FollowerBuffer);
                    printf("Invalid command\n");
                }
            }else{
                printf("recvfrom() error\n");
            }
        }
    }


	return 0;
}
