#include "../include/unixnet2mac.h"

#include "internal.hpp"

#include "OpenTransport.h"

int connect(int socket, const struct sockaddr *address, socklen_t address_len) {
  OSStatus err;
  auto s = openSockets.at(socket);

  ThrowOSErr(OTBIND(s->endpoint, nil, nil));

  OTMemzero(&s->sndCall, sizeof(TCall));
  s->sndCall->addr.buf = (UInt8 *)&s->hostDNSAddress;
  s->sndCall->addr.len =
      OTINITDNSADDRESS(&s->hostDNSAddress, (char *)address->sa_data);

  ThrowOSErr(OTCONNECT(s->endpoint, s->sndCall, s->rcvCall));

  return 0;
};
