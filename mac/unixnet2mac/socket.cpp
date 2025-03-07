#include "MacTypes.h"
#include "OpenTransport.h"
#include "OpenTransportProviders.h"
#include "Threads.h"
#include "internal.hpp"
#include "unixnet2mac.h"
#include <cstddef>

// commented out to shut clangd up
// #include <sys/socket.h>

int socket(int domain, int type, int protocol) {
  OSErr err = noErr;
  if (err = InitOpenTransport(); err != noErr) {
    mac_error_throw("Error initializing OpenTransport: %d\n", err);
  }
  YieldToAnyThread();

  Socket *skt = new Socket();
  YieldToAnyThread();

  switch (type) {
  case SOCK_STREAM: // tcp
    skt->cfg = OTCreateConfiguration(kTCPName);
    break;
  case SOCK_DGRAM: // udp
    skt->cfg = OTCreateConfiguration(kUDPName);
    break;
  default:
    mac_error_throw("Unknown socket type \"%d\" passed", type);
  }
  YieldToAnyThread();

  // OTMemzero(&skt->info, sizeof(skt->info));

  OSStatus OTEndpointStatus = 0;
  skt->endpoint = OTOpenEndpoint(skt->cfg, 0, nil, &OTEndpointStatus);
  ThrowOSErr(OTEndpointStatus);
  YieldToAnyThread();

  OTUseSyncIdleEvents(skt->endpoint, false);
  YieldToAnyThread();

  size_t socketId = openSockets.size();
  OPEN_SOCKET_INSERT(socketId, skt);

  YieldToAnyThread();

  return socketId;
};
