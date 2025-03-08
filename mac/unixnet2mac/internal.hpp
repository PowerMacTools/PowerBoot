
#include "MacTypes.h"
#include "OpenTransport.h"
#include "OpenTransportProviders.h"
#include "Threads.h"
#include <cstdio>
#include <unordered_map>

#ifndef __INTERNAL_HPP
#define __INTERNAL_HPP

class Socket {

public:
  size_t id;
  TEndpoint *endpoint = NULL;
  OTConfiguration *cfg = NULL;
  TEndpointInfo info = {0, 0, 0, 0, 0, 0, 0, 0};
  InetSvcRef inetsvc = NULL;

  DNSAddress hostDNSAddress = {0};
  TCall sndCall = {0};

  TBind *retAddr = NULL;

  ~Socket();
};

extern std::unordered_map<size_t, Socket *> openSockets;

void __throw_os_err(const char *file, int line, const char *func, OSErr err);
#define ThrowOSErr(err) __throw_os_err(__FILE__, __LINE__, #err, err)

#define OPEN_SOCKET_INSERT(x, y)                                               \
  openSockets.insert(std::pair<size_t, Socket *>(x, y))

void mac_error_throw(const char *format, ...);

extern ThreadID main_thread_id;

#endif