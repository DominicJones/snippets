// Client side C/C++ program to demonstrate Socket programming
// https://www.geeksforgeeks.org/socket-programming-cc/

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char const *argv[])
{
  int cs_socket = 0;
  struct sockaddr_in serv_addr;

  if ((cs_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("\n Socket creation error \n");
    return -1;
  }

  {
    int port = 8080;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
  }

  // Convert IPv4 and IPv6 addresses from text to binary form
  if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
  {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }

  if (connect(cs_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("\nConnection Failed \n");
    return -1;
  }

  {
    char *msg = "Message from client";
    char buffer[1024] = {0};
    int valread;
    send(cs_socket, msg, strlen(msg), 0);
    valread = read(cs_socket, buffer, 1024);
    printf("[CLIENT] %s\n", buffer);
  }

  return 0;
}
