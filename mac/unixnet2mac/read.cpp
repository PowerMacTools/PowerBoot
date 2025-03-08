#include "MacErrors.h"
#include "OpenTransport.h"
#include "internal.hpp"
#include "unixnet2mac.h"
#include <cerrno>

ssize_t recv(int fd, void *buf, size_t nbytes, int flags) {
  auto s = openSockets.at(fd);
  OTResult ret = kOTNoDataErr;
  OTFlags ot_flags = 0;

  YieldToAnyThread();

  if (nbytes == 0)
    return 0;

  // in non-blocking mode, returns instantly always
  ret = OTRcv(s->endpoint, buf, nbytes, &ot_flags);

  YieldToAnyThread();

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

ssize_t send(int fd, const void *buf, size_t nbytes, int flags) {
  auto s = openSockets.at(fd);
  int ret = -1;

  YieldToAnyThread();

  ret = OTSnd(s->endpoint, (void *)buf, nbytes, 0);

  YieldToAnyThread();

  if (ret != kOTNoError) {
    if (ret == kOTLookErr) {
      return -1;
    } else {
      ThrowOSErr(ret);
    }
  }

  return (ssize_t)ret;
}
