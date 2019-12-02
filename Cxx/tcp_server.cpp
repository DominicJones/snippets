// Server side C/C++ program to demonstrate Socket programming
// https://www.geeksforgeeks.org/socket-programming-cc/

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

int main(int argc, char const *argv[])
{
  int server_fd;
  struct sockaddr_in address;

  // Creating socket file descriptor
  int protocol = 0; // Internet Protocol
  if ((server_fd = socket(AF_INET, SOCK_STREAM, protocol)) == 0)
  {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Set socket options
  int opt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
  {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  int port = 8080;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY; // localhost
  address.sin_port = htons(port);

  // Forcefully attaching socket to the address and port
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
  {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  int backlog = 3; // length which queue of pending connections for socket may grow
  if (listen(server_fd, backlog) < 0)
  {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  int cs_socket;
  int addrlen = sizeof(address);
  if ((cs_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
  {
    perror("accept");
    exit(EXIT_FAILURE);
  }

  {
    char buffer[1024] = {0};
    char * msg = "Message from server";
    int valread;
    valread = read(cs_socket, buffer, 1024);
    printf("[SERVER] %s\n", buffer);
    send(cs_socket, msg, strlen(msg), 0);
  }

  return 0;
}
