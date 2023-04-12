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

	int  maxlen = 1024;
	int n = 0;
	char RecvBuffer[maxlen];
  char SendBuffer[maxlen];

  while (1) {
      n = recvfrom(sock, &RecvBuffer, sizeof(RecvBuffer), 0, (struct sockaddr *)&cli_addr, &cli_addr_len);
      if (n > 0) {
	      RecvBuffer[n]='\0';
	      printf("%s", RecvBuffer);
	      char cmd[10], key[maxlen], value[maxlen];
	      int num_fields = sscanf(RecvBuffer, "%s %s %s", cmd, key, value);
	      
	      if(strncmp(cmd, "put", 3)==0){
		      char *p=strtok(RecvBuffer," ");
		      if(p==NULL) {
			      sprintf(SendBuffer, "Invalid command");
			      printf("Invalid command\n");
		      }else{
			      char *key = strtok(NULL, " ");
			      if(key==NULL){
				      sprintf(SendBuffer, "Invalid command");
				      printf("Invalid command\n");
			      }else{
				      char *value = strtok(NULL, " ");
				      if(value==NULL){
					      sprintf(SendBuffer, "Invalid command");
					      printf("Invalid command\n");
				      } else{
					      char *extra = strtok(NULL, " ");
					      if(extra != NULL){
						      sprintf(SendBuffer, "Invalid command");
						      printf("Invalid command\n");
					      } else {
					      	put(key, value);
					      	sprintf(SendBuffer, "OK");
						printf("PUT is done\n");
					      }
				      }
			      }
		      }
	      } else if(num_fields==2 && strcmp(cmd, "get")==0 && strlen(key)>0){
		      const char *result = get(key);
		      if(result==NULL) {
			      sprintf(SendBuffer, "Key not found");
			      printf("Key not found. GET failed.\n");
		      }else {
			      char *newline = strchr(result, '\n');
			      if(newline){
				      *newline = '\0';
			      }
			      sprintf(SendBuffer, "%s", result);
			      printf("GET is done.\n");
		      }
	      }else if (num_fields==3 && strcmp(cmd, "put")==0 && strlen(key)>0 && strlen(value)>0){
		      put(key, value);
		      sprintf(SendBuffer, "OK");
		      printf("PUT is done\n");
	      }else {
		      sprintf(SendBuffer, "Invalid command");
		      printf("Invalid command\n");
	      }
	      sendto(sock, SendBuffer, strlen(SendBuffer), 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
      }
  }
	close(sock);

	return 0;
}
