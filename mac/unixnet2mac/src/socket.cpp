#include "../include/unixnet2mac.h"
#include "MacTypes.h"
#include "OpenTransport.h"
#include "OpenTransportProviders.h"
#include "internal.hpp"
#include <cstddef>
// commented out to shut clangd up
// #include <sys/socket.h>

int socket(int domain, int type, int protocol) {
  OSErr err = noErr;
  if (err = InitOpenTransport(); err != noErr) {
    mac_error_throw("Error initializing OpenTransport: %d\n", err);
  }

  Socket *skt = new Socket();

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

  // OTMemzero(&skt->info, sizeof(skt->info));

  OSStatus OTEndpointStatus = 0;
  skt->endpoint = OTOpenEndpoint(skt->cfg, 0, nil, &OTEndpointStatus);
  ThrowOSErr(OTEndpointStatus);

  OTSetSynchronous(skt->endpoint);
  OTSetBlocking(skt->endpoint);
  OTUseSyncIdleEvents(skt->endpoint, false);
  
  size_t socketId = openSockets.size();
  OPEN_SOCKET_INSERT(socketId, skt);
  return socketId;
};
