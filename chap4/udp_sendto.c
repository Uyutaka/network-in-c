#include "chap4.h"
int main() {
  printf("Configuring remote address...\n");
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_DGRAM;
  struct addrinfo *peer_address;
  if (getaddrinfo("127.0.0.1", "8080", &hints, &peer_address)) {
    fprintf(stderr, "getaddrinfo() failed. (%d)\n", errno);
    return 1;
  }

  printf("Remote address is: ");
  char address_buffer[100];
  char service_buffer[100];
  getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen, address_buffer,
              sizeof(address_buffer), service_buffer, sizeof(service_buffer),
              NI_NUMERICHOST | NI_NUMERICSERV);
  printf("%s %s\n", address_buffer, service_buffer);

  printf("Creating socket...\n");
  int socket_peer;
  socket_peer = socket(peer_address->ai_family, peer_address->ai_socktype,
                       peer_address->ai_protocol);
  if (socket_peer < 0) {
    fprintf(stderr, "socket() failed. (%d)\n", errno);
    return 1;
  }

  const char *message = "Hello World";
  printf("Sending: %s\n", message);
  int bytes_sent = sendto(socket_peer, message, strlen(message), 0,
                          peer_address->ai_addr, peer_address->ai_addrlen);
  printf("Send %d bytes.\n", bytes_sent);

  freeaddrinfo(peer_address);
  close(socket_peer);

  printf("Finished.\n");
  return 0;
}
