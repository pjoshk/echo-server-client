#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "form.c"


int main() {
  char client_msg[1024];
  // char server_msg[1024];

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

  renderWindow(client_fd);

  return 0;
}
