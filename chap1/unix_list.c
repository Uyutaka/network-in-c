#include <ifaddrs.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

int main() {
  struct ifaddrs *addresses;
  // getifaddrs()
  // https://man7.org/linux/man-pages/man3/getifaddrs.3.html
  /*
    *ifa_next;    Next item in list
    *ifa_name;    Name of interface
    ifa_flags;   Flags from SIOCGIFFLAGS
    *ifa_addr;    Address of interface
    *ifa_netmask;  Netmask of interface
  */

  if (getifaddrs(&addresses) == -1) {
    printf("getifaddrs call failed\n");
    return -1;
  }
  struct ifaddrs *address = addresses;
  while (address) {
    int family = address->ifa_addr->sa_family;
    if (family == AF_INET || family == AF_INET6) {
      printf("%s\t", address->ifa_name);
      printf("%s\t", family == AF_INET ? "IPv4" : "IPv6");
      char ap[100];
      const int family_size = family == AF_INET ? sizeof(struct sockaddr_in)
                                                : sizeof(struct sockaddr_in6);

      // getnameinfo()
      // https://man7.org/linux/man-pages/man3/getnameinfo.3.html
      getnameinfo(address->ifa_addr, family_size, ap, sizeof(ap), 0, 0,
                  NI_NUMERICHOST);
      printf("\t%s\n", ap);
    }
    address = address->ifa_next;
  }
}
