#include "MacErrors.h"
#include "OpenTransport.h"
#include "Threads.h"
#include "internal.hpp"
#include "sys/socket.h"
#include <cerrno>
#include <cstring>
#include <string>

extern "C" ssize_t send(int fd, const void *buf, size_t nbytes, int flags) {
  auto s = openSockets.at(fd);

  printf("<%8ld...", nbytes);

  // OTResult ret = kOTNoDataErr;
  // OTFlags ot_flags = 0;

  // YieldToAnyThread();

  // if (nbytes == 0)
  //   return 0;

  // // in non-blocking mode, returns instantly always
  // s->send_halt = true;
  OSErr OTSndResult = OTSnd(s->endpoint, (void *)buf, nbytes, 0);
  ThrowOSErr(OTSndResult);

  if (nbytes <= 64) {
    printf("%s\n", std::string((char *)buf, (char *)buf + nbytes).c_str());
    YieldToAnyThread();
  } else {
    printf("\n");
    YieldToAnyThread();
  }

  YieldToAnyThread();

  return OTSndResult;

  // If ret isn't a no data error, we're in synchronous mode and should just
  // return whatever we got.
  // if (ret != kOTFlowErr) {
  //   s->send_halt = false;
  //   return ret;
  // }
  // printf("asked to send %ld bytes\n", nbytes);
  // while (s->send_halt) {
  //   YieldToAnyThread();
  // }

  // auto placeholder = flatten(s->sendBuf);
  // s->sendBuf.erase(s->sendBuf.begin(), s->sendBuf.end());

  // YieldToAnyThread();

  // return placeholder.size();
}
