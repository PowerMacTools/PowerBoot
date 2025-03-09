#include "arpa/inet.h"

// On both 68k and PowerPC, the endianness is the same as the network byte
// order. So these functions become pretty easy to implement.

uint32_t htonl(uint32_t hostlong) { return hostlong; };
uint16_t htons(uint16_t hostshort) { return hostshort; };
uint32_t ntohl(uint32_t netlong) { return netlong; };
uint16_t ntohs(uint16_t netshort) { return netshort; };