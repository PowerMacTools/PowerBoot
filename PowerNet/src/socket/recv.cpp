#include "MacErrors.h"
#include "OpenTransport.h"
#include "internal.hpp"
#include "sys/socket.h"
#include <cerrno>

extern "C" ssize_t recv(int fd, void *buf, size_t nbytes, int flags) {
  OTMemzero(buf, nbytes);
  auto s = openSockets.at(fd);
  OTResult ret = kOTNoDataErr;
  OTFlags ot_flags = 0;

  YieldToAnyThread();

  if (nbytes == 0)
    return 0;

  // in non-blocking mode, returns instantly always
  ret = OTRcv(s->endpoint, buf, nbytes, &ot_flags);

  // if we got bytes, return them
  if (ret >= 0)
    return ret;

  // if no data, let other threads run, then tell caller to call again
  if (ret == kOTNoDataErr) {
    YieldToAnyThread();
    return -EAGAIN;
  }

  // if we got anything other than data or nothing, return an error
  return -1;
}
