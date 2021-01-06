// Based on
// https://github.com/codeplea/Hands-On-Network-Programming-with-C/blob/master/chap03/tcp_client.c
// Explaination
// https://learning.oreilly.com/library/view/hands-on-network-programming/9781789349863/691fb6f0-705f-4e3f-8657-7e8bd2e39e1f.xhtml
#include "chap3.c"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    // ./tcp_client example.com http
    fprintf(stderr, "usage: tcp_client hostname port\n");
    return 1;
  }

  printf("Configuring remote address...\n");
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;  // TCP
  struct addrinfo *peer_address;
  if (getaddrinfo(argv[1], argv[2], &hints, &peer_address)) {
    fprintf(stderr, "getaddrinfo() failed. (%d)\n", errno);
    return 1;
  }

  printf("Remote adress is: ");
  char address_buffer[100];
  char service_buffer[100];
  getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen, address_buffer,
              sizeof(address_buffer), service_buffer, sizeof(service_buffer),
              NI_NUMERICHOST);
  printf("%s %s\n", address_buffer, service_buffer);

  printf("Creating socket...\n");
  SOCKET socket_peer;
  socket_peer = socket(peer_address->ai_family, peer_address->ai_socktype,
                       peer_address->ai_protocol);
  if (socket_peer < 0) {
    fprintf(stderr, "socket() failed. (%d)\n", errno);
    return 1;
  }

  printf("Connecting...\n");
  if (connect(socket_peer, peer_address->ai_addr, peer_address->ai_addrlen)) {
    fprintf(stderr, "connect() failed. (%d)\n", errno);
    return 1;
  }

  freeaddrinfo(peer_address);

  printf("Connected!\n");
  printf("To send data, enter text followed by enter.\n");
  while (1) {
    fd_set reads;
    FD_ZERO(&reads);
    FD_SET(socket_peer, &reads);
    FD_SET(0, &reads);  // === FD_SET(fileno(stdin), &reads)

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    if (select(socket_peer + 1, &reads, 0, 0, &timeout) < 0) {
      fprintf(stderr, "select() failed. (%d)\n", errno);
      return 1;
    }

    if (FD_ISSET(socket_peer, &reads)) {
      char read[4096];
      int bytes_received = recv(socket_peer, read, 4096, 0);
      if (bytes_received < 1) {
        printf("Connection closed by peer.\n");
        break;
      }
      printf("Received (%d bytes): %.*s", bytes_received, bytes_received, read);
    }

    // enable cat my_file.txt | tcp_client 192.168.54.122 8080.
    if (FD_ISSET(0, &reads)) {
      char read[4096];
      if (!fgets(read, 4096, stdin)) break;
      printf("Sending: %s", read);
      int bytes_sent = send(socket_peer, read, strlen(read), 0);
      printf("Sent %d bytes.\n", bytes_sent);
    }
  }
  printf("Closing socket...\n");
  close(socket_peer);
  printf("Finished.\n");
  return 0;
}
