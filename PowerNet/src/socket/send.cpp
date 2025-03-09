#include "MacErrors.h"
#include "OpenTransport.h"
#include "internal.hpp"
#include "sys/socket.h"
#include <cerrno>

extern "C" ssize_t send(int fd, const void *buf, size_t nbytes, int flags) {
  auto s = openSockets.at(fd);

  int ret = -1;

  YieldToAnyThread();

  ret = OTSnd(s->endpoint, (void *)buf, nbytes, 0);

  if (ret != kOTNoError) {
    if (ret == kOTLookErr) {
      return -EAGAIN;
    } else {
      ThrowOSErr(ret);
    }
  }

  return (ssize_t)ret;
}
