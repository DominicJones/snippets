// Server side C/C++ program to demonstrate Socket programming
// https://www.geeksforgeeks.org/socket-programming-cc/

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <getopt.h>

#include <iostream>


bool find_port = true;
int port = 0; // find a free socket port

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
      find_port = false;
      std::cout << "Using port: " << port << std::endl;
      break;

    default:
      break;
    }
  }
}


int main(int argc, char** argv)
{
  get_optional_args(argc, argv);

  int server_fd;
  struct sockaddr_in address;

  // Creating socket file descriptor
  {
    int protocol = 0; // Internet Protocol
    if ((server_fd = socket(AF_INET, SOCK_STREAM, protocol)) == 0)
    {
      perror("socket failed");
      exit(EXIT_FAILURE);
    }
  }

  // Set socket options
  {
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
      perror("setsockopt");
      exit(EXIT_FAILURE);
    }
  }

  {
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // localhost
    address.sin_port = htons(port);
  }

  // Forcefully attaching socket to the address and port
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
  {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  if (find_port)
  {
    int addrlen = sizeof(address);
    if (getsockname(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen) < 0)
    {
      perror("getsockname failed");
      exit(EXIT_FAILURE);
    }

    int port = ntohs(address.sin_port);
    std::cout << "Using port: " << port << std::endl;
  }

  {
    int backlog = 3; // length which queue of pending connections for socket may grow
    if (listen(server_fd, backlog) < 0)
    {
      perror("listen");
      exit(EXIT_FAILURE);
    }
  }

  int cs_socket;
  {
    int addrlen = sizeof(address);
    if ((cs_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
    {
      perror("accept");
      exit(EXIT_FAILURE);
    }
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
