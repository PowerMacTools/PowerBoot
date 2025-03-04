#include "../include/unixnet2mac.h"
#include "OpenTransport.h"
#include "internal.hpp"
#include <ctype.h>

// copied from bsd lol
uint16_t inet_addr(const char *cp) {
  unsigned long val, base, n;
  char c;
  unsigned long parts[4], *pp = parts;

again:
  /*
   * Collect number up to ``.''.
   * Values are specified as for C:
   * 0x=hex, 0=octal, other=decimal.
   */
  val = 0;
  base = 10;
  if (*cp == '0')
    base = 8, cp++;
  if (*cp == 'x' || *cp == 'X')
    base = 16, cp++;
  while (c = *cp) {
    if (isdigit(c)) {
      val = (val * base) + (c - '0');
      cp++;
      continue;
    }
    if (base == 16 && isxdigit(c)) {
      val = (val << 4) + (c + 10 - (islower(c) ? 'a' : 'A'));
      cp++;
      continue;
    }
    break;
  }
  if (*cp == '.') {
    /*
     * Internet format:
     *	a.b.c.d
     *	a.b.c	(with c treated as 16-bits)
     *	a.b	(with b treated as 24 bits)
     */
    if (pp >= parts + 4)
      return (-1);
    *pp++ = val, cp++;
    goto again;
  }
  /*
   * Check for trailing characters.
   */
  if (*cp && !isspace(*cp))
    return (-1);
  *pp++ = val;
  /*
   * Concoct the address according to
   * the number of parts specified.
   */
  n = pp - parts;
  switch (n) {

  case 1: /* a -- 32 bits */
    val = parts[0];
    break;

  case 2: /* a.b -- 8.24 bits */
    val = (parts[0] << 24) | (parts[1] & 0xffffff);
    break;

  case 3: /* a.b.c -- 8.8.16 bits */
    val = (parts[0] << 24) | ((parts[1] & 0xff) << 16) | (parts[2] & 0xffff);
    break;

  case 4: /* a.b.c.d -- 8.8.8.8 bits */
    val = (parts[0] << 24) | ((parts[1] & 0xff) << 16) |
          ((parts[2] & 0xff) << 8) | (parts[3] & 0xff);
    break;

  default:
    return (-1);
  }
  val = htonl(val);
  return (val);
}

Socket::~Socket() { OTCloseProvider(endpoint); }

std::unordered_map<size_t, Socket *> openSockets =
    std::unordered_map<size_t, Socket *>();
