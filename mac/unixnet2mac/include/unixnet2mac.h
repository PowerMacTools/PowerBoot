#include <stdint.h>

#ifndef __UNIXNET2MAC__H
#define __UNIXNET2MAC__H

#ifdef __cplusplus
extern "C" {
#endif

#define AF_UNSPEC 0
#define AF_UNIX 1      /* Unix domain sockets 		*/
#define AF_INET 2      /* Internet IP Protocol 	*/
#define AF_AX25 3      /* Amateur Radio AX.25 		*/
#define AF_IPX 4       /* Novell IPX 			*/
#define AF_APPLETALK 5 /* Appletalk DDP 		*/
#define AF_NETROM 6    /* Amateur radio NetROM 	*/
#define AF_BRIDGE 7    /* Multiprotocol bridge 	*/
#define AF_AAL5 8      /* Reserved for Werner's ATM 	*/
#define AF_X25 9       /* Reserved for X.25 project 	*/
#define AF_INET6 10    /* IP version 6			*/
#define AF_MAX 12      /* For now.. */

#define PF_UNSPEC AF_UNSPEC
#define PF_UNIX AF_UNIX
#define PF_INET AF_INET
#define PF_AX25 AF_AX25
#define PF_IPX AF_IPX
#define PF_APPLETALK AF_APPLETALK
#define PF_NETROM AF_NETROM
#define PF_BRIDGE AF_BRIDGE
#define PF_AAL5 AF_AAL5
#define PF_X25 AF_X25
#define PF_INET6 AF_INET6

#define PF_MAX AF_MAX

#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_RAW 3
#define SOCK_RDM 4
#define SOCK_CONN_DGRAM 5

typedef unsigned int socklen_t;
typedef uint16_t sa_family_t;

struct in_addr {
  unsigned long s_addr; // load with inet_aton()
};

struct sockaddr_in {
  short sin_family;        // e.g. AF_INET
  unsigned short sin_port; // e.g. htons(3490)
  struct in_addr sin_addr; // see struct in_addr, below
  char sin_zero[8];        // zero this if you want to
};

struct sockaddr {
  sa_family_t sa_family; /* Address family */
  char sa_data[];        /* Socket address */
};

uint16_t inet_addr(const char *str);
int connect(int socket, const struct sockaddr *address, socklen_t address_len);
int socket(int domain, int type, int protocol);

// It just so happens that "network byte order" is the same as the endianness of
// both PPC and 68k

#define htonl(x) x
#define htons(x) x
#define ntohl(x) x
#define ntohs(x) x

#ifdef __cplusplus
}
#endif

#endif
