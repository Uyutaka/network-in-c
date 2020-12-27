// Explanation
// https://learning.oreilly.com/library/view/hands-on-network-programming/9781789349863/787c22a4-c7c1-4b20-b047-6b88f58b2494.xhtml
// Code
// https://github.com/codeplea/Hands-On-Network-Programming-with-C/blob/master/chap02/time_server.c

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)

int main() {
  printf("Configuring local adddress...\n");
  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;        // IPv4
  hints.ai_socktype = SOCK_STREAM;  // TCP
  hints.ai_flags = AI_PASSIVE;      // listen on any available interface

  struct addrinfo *bind_address;
  // https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
  getaddrinfo(0, "8080", &hints, &bind_address);

  printf("Creating socket...\n");
  int socket_listen;
  socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype,
                         bind_address->ai_protocol);

  // When the socket is invalid
  if (socket_listen < 0) {
    fprintf(stderr, "socket() failed. (%d)\n", errno);
    return 1;
  }

  printf("Binding socket to local address...\n");
  if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen)) {
    fprintf(stderr, "bind() failed. (%d)\n", errno);
    return 1;
  }

  freeaddrinfo(bind_address);

  printf("Listening...\n");
  if (listen(socket_listen, 10) < 0) {
    fprintf(stderr, "listen() failed. (%d)\n", errno);
    return 1;
  }

  printf("Waiting for connection...\n");
  struct sockaddr_storage client_address;  // Store the info for client
  socklen_t client_len = sizeof(client_address);
  int socket_client =
      accept(socket_listen, (struct sockaddr *)&client_address, &client_len);
  if (socket_client < 0) {
    fprintf(stderr, "accept() failed. (%d)\n", errno);
    return 1;
  }

  printf("Client is connected...\n");
  // For logging
  char address_buffer[100];
  getnameinfo((struct sockaddr *)&client_address, client_len, address_buffer,
              sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
  printf("%s\n", address_buffer);

  printf("Reading request...\n");
  char request[1024];
  int bytes_received = recv(socket_client, request, 1024, 0);
  printf("Received %d bytes.\n", bytes_received);
  printf("%.*s", bytes_received, request);

  printf("Sending response...\n");
  const char *response =
      "HTTP/1.1 200 OK\r\n"
      "Connection: close\r\n"
      "Content-Type: text/plain\r\n\r\n"
      "Local time is: ";
  int bytes_sent = send(socket_client, response, strlen(response), 0);
  printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(response));

  time_t timer;
  time(&timer);
  char *time_msg = ctime(&timer);
  bytes_sent = send(socket_client, time_msg, strlen(time_msg), 0);
  printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(time_msg));

  printf("Closing connection...\n");
  close(socket_client);

  printf("Closing listening socket...\n");
  close(socket_listen);

  printf("Finished.\n");
  return 0;
}
