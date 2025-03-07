#include "OpenTransport.h"
#include "internal.hpp"
#include "unixnet2mac.h"

ssize_t recv(int fd, void *buf, size_t nbytes, int flags) {
  auto s = openSockets.at(fd);
  OTFlags f = T_MORE;
  ssize_t fin = 0;

  while (f != T_MORE) {
    int OTRcvResult = OTRcv(s->endpoint, buf, nbytes, &f);
    if (OTRcvResult < 0) {
      ThrowOSErr(OTRcvResult);
    } else {
      fin += OTRcvResult;
    }
  }

  return fin;
}

ssize_t send(int fd, const void *buf, size_t nbytes, int flags) {
  auto s = openSockets.at(fd);

  int OTSndResult = OTSnd(s->endpoint, (void *)buf, nbytes, T_EXPEDITED);

  if (OTSndResult < 0) {
    ThrowOSErr(OTSndResult);
    return 0;
  } else {
    return OTSndResult;
  }
}
