// Client side C/C++ program to demonstrate Socket programming
// https://www.geeksforgeeks.org/socket-programming-cc/

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include <iostream>


int port = 8080; // default port

void get_optional_args(int argc, char** argv)
{
  const char* const short_opts = "p";
  const option long_opts[] = {
    {"port", required_argument, nullptr, 'p'},
    {nullptr, no_argument, nullptr, 0}
  };

  while (true)
  {
    const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);
    if (opt == -1)
      break;

    switch (opt)
    {
    case 'p':
      port = std::stoi(optarg);
      break;

    default:
      break;
    }
  }
}


int main(int argc, char** argv)
{
  get_optional_args(argc, argv);
  std::cout << "Using port: " << port << std::endl;

  int cs_socket = 0;
  struct sockaddr_in serv_addr;

  if ((cs_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("\n Socket creation error \n");
    return -1;
  }

  {
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
