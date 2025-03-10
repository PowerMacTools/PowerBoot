
#include "MacTypes.h"
#include "OpenTransport.h"
#include "OpenTransportProviders.h"
#include "Threads.h"
#include <cstdint>
#include <cstdio>
#include <unordered_map>
#include <vector>

#ifndef __INTERNAL_HPP
#define __INTERNAL_HPP

class Socket {

public:
  bool halting = false;
  bool recv_halt = false;
  bool send_halt = false;

  size_t id = -1;
  size_t nbytes = -1;
  TEndpoint *endpoint = NULL;
  OTConfiguration *cfg = NULL;
  TEndpointInfo info = {0, 0, 0, 0, 0, 0, 0, 0};
  InetSvcRef inetsvc = NULL;

  std::vector<std::vector<uint8_t>> recvBuf =
      std::vector<std::vector<uint8_t>>();
  std::vector<std::vector<uint8_t>> sendBuf =
      std::vector<std::vector<uint8_t>>();

  DNSAddress hostDNSAddress = {0};
  TCall sndCall = {0};

  TBind *retAddr = NULL;

  ~Socket();
};

extern std::unordered_map<size_t, Socket *> openSockets;

void __throw_os_err(const char *file, int line, const char *func, OSErr err);
#define ThrowOSErr(err) __throw_os_err(__FILE__, __LINE__, #err, err)

int __mem_test(const char *file, int line, void *ptr, bool _throw);
#define MemTest(ptr, _throw) __mem_test(__FILE__, __LINE__, ptr, _throw)

#define OPEN_SOCKET_INSERT(x, y)                                               \
  openSockets.insert(std::pair<size_t, Socket *>(x, y))

void mac_error_throw(const char *format, ...);

extern ThreadID main_thread_id;

std::vector<uint8_t> flatten(const std::vector<std::vector<uint8_t>> &v);

#endif