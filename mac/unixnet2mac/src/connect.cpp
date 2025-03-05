#include "../include/unixnet2mac.h"

#include "internal.hpp"

#include "OpenTransport.h"
#include <cstddef>
#include <cstdlib>

int connect(int socket, const struct sockaddr *address, socklen_t address_len) {
  sockaddr_in *dafaq = (sockaddr_in *)address;
  OSStatus err;
  auto s = openSockets.at(socket);

  ThrowOSErr(OTBind(s->endpoint, nil, nil));

#define ADDR_SIZE sizeof(dafaq->sin_addr.s_addr)
  char *ipStr = (char *)malloc(ADDR_SIZE * 4);
  size_t og = (size_t)ipStr;

  for (int i = 0; i < ADDR_SIZE; i++) {
    auto val = dafaq->sin_addr.s_addr << (i * 8) >> 24;
    s->sndCall.addr.buf[i] = val;
    if (i == ADDR_SIZE - 1) {
      ipStr += snprintf(ipStr, 4, "%d", val);
    } else {
      ipStr += snprintf(ipStr, 5, "%d.", val);
    }
  }
  ipStr = (char *)og;

  OTMemzero(&s->sndCall, sizeof(TCall));
  s->sndCall.addr.buf = (UInt8 *)&s->hostDNSAddress;
  s->sndCall.addr.len = OTInitDNSAddress(&s->hostDNSAddress, ipStr);

  printf("Connecting: %s\n", s->sndCall.addr.buf + 2);
  ThrowOSErr(OTConnect(s->endpoint, &s->sndCall, nil));

  return 0;
};
