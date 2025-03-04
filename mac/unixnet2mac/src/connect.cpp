#include "../include/unixnet2mac.h"
#include "MacErrors.h"
#include "OpenTransport.h"
#include "internal.hpp"

int connect(int socket, const struct sockaddr *address, socklen_t address_len) {
  OSStatus err;
  auto s = openSockets.at(socket);

  ThrowOSErr(OTBind(s->endpoint, nil, nil));

  OTMemzero(&s->sndCall, sizeof(TCall));
  s->sndCall->addr.buf = (UInt8 *)&s->hostDNSAddress;
  s->sndCall->addr.len =
      OTInitDNSAddress(&s->hostDNSAddress, (char *)address->sa_data);

  ThrowOSErr(OTConnect(s->endpoint, s->sndCall, s->rcvCall));

  return 0;
};
