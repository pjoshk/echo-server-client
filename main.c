#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "form.c"

#define MAX_BUFFER_SIZE 1024

typedef struct serverOpts {
  int client_fd;
  char* server_msg;
} serverOpts;

void* read_server_msg(void* server_otps){
  serverOpts* opts = (struct serverOpts*) server_otps;
  int client_fd = opts->client_fd;
  char* server_msg = opts->server_msg;

  while(1){
    int bytes_received = recv(client_fd, server_msg, MAX_BUFFER_SIZE, 0);
    if(client_fd < 0){
        break;
    }
  }

  return NULL;
}

int main() {
  char client_msg[MAX_BUFFER_SIZE];
  char server_msg[MAX_BUFFER_SIZE];
  pthread_t thread_id;

  // create server address
  struct sockaddr_in server_addr = {
    .sin_family = AF_INET,
    // .sin_addr.s_addr = inet_addr("51.20.135.212"),
    .sin_addr.s_addr = inet_addr("127.0.0.1"),
    .sin_port = htons(6969)
  };

  // create client socket
  int client_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (client_fd < 0) {
    perror("[DEBUG][ERROR] socket failed\n");
    exit(EXIT_FAILURE);
  }

  // connect to the server
  int connection = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (connection < 0){
    perror("[DEBUG][ERROR] socket failed\n");
    exit(EXIT_FAILURE);
  }

  serverOpts server_opts = {
    .client_fd = client_fd,
    .server_msg = server_msg
  };

  pthread_create(&thread_id, NULL, read_server_msg, &server_opts);

  renderWindow(client_fd, server_msg);

  return 0;
}
