#include "../include/unixnet2mac.h"
#include "OpenTransport.h"
#include "internal.hpp"
#include <cstddef>
#include <stdexcept>
// commented out to shut clangd up
// #include <sys/socket.h>

int socket(int domain, int type, int protocol) {
  OSStatus stat;

  Socket *skt = new Socket();

  switch (type) {
  case SOCK_STREAM: // tcp
    skt->cfg = OTCreateConfiguration("tcp");
    break;
  case SOCK_DGRAM: // udp
    skt->cfg = OTCreateConfiguration("udp");
    break;
  default:
    throw std::runtime_error("Unknown socket type passed");
  }

  skt->endpoint = OTOpenEndpoint(skt->cfg, 0, &skt->info, &stat);

  ThrowOSErr(stat);

  size_t socketId = openSockets.size();
  OPEN_SOCKET_INSERT(socketId, skt);
  return socketId;
};
