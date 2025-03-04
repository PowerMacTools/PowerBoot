
#include "MacTypes.h"
#include "OpenTransport.h"
#include "OpenTransportProviders.h"
#include <cstdio>
#include <unordered_map>
#include <utility>

#ifndef __INTERNAL_HPP
#define __INTERNAL_HPP

class Socket {

public:
  EndpointRef endpoint;
  OTConfigurationRef cfg;
  TEndpointInfo info;

  DNSAddress hostDNSAddress;
  TCall *sndCall = NULL;
  TCall *rcvCall = NULL;

  TBind *retAddr;

  ~Socket();
};

extern std::unordered_map<size_t, Socket *> openSockets;

void ThrowOSErr(OSErr err);

#define OPEN_SOCKET_INSERT(x, y)                                               \
  openSockets.insert(std::pair<size_t, Socket *>(x, y))

#endif