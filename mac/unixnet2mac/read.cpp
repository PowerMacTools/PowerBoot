#include "MacErrors.h"
#include "OpenTransport.h"
#include "internal.hpp"
#include "unixnet2mac.h"
#include <cerrno>

ssize_t recv(int fd, void *buf, size_t nbytes, int flags) {
  auto s = openSockets.at(fd);
  OTResult ret = kOTNoDataErr;
  OTFlags ot_flags = 0;

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
  if (ret != kOTNoDataErr)
    return -1;

  return -1;
}

ssize_t send(int fd, const void *buf, size_t nbytes, int flags) {
  auto s = openSockets.at(fd);
  int ret = -1;

  ret = OTSnd(s->endpoint, (void *)buf, nbytes, 0);

  // TODO FIXME handle cases better, i.e. translate error cases
  if (ret == kOTLookErr) {
    OTResult lookresult = OTLook(s->endpoint);
    // printf("kOTLookErr, reason: %ld\n", lookresult);

    switch (lookresult) {
    default:
      // printf("what?\n");
      ret = -1;
      break;
    }
  }

  return (ssize_t)ret;
}
