#include "MacErrors.h"
#include "OpenTransport.h"
#include "Threads.h"
#include "internal.hpp"
#include "sys/socket.h"
#include <cerrno>
#include <cstring>
#include <string>

extern "C" ssize_t recv(int fd, void *buf, size_t nbytes, int flags) {
  OTMemzero(buf, nbytes);
  auto s = openSockets.at(fd);
  OTResult ret = kOTNoDataErr;
  OTFlags ot_flags = 0;

  YieldToAnyThread();

  // in non-blocking mode, returns instantly always
  ret = OTRcv(s->endpoint, buf, nbytes, &ot_flags);

  // If ret isn't a no data error, we're in synchronous mode and should just
  // return whatever we got.
  if (ret != kOTNoDataErr) {
    s->recv_halt = false;
    return ret;
  }

  printf("asked to recv %ld bytes\n", nbytes);

  while (s->recv_halt) {
    YieldToAnyThread();
  }

  auto fin = flatten(s->recvBuf);

  memcpy(buf, fin.data(), fin.size());
  auto ass = fin.size();
  s->recvBuf.erase(s->recvBuf.begin(), s->recvBuf.end());

  printf("recv: %ld\n", fin.size());

  // if we got bytes, return them
  if (fin.size() >= 0) {
    return fin.size();
  }
  // if we got anything other than data or nothing, return an error
  return -1;
}
