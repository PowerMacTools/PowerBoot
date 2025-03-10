#include "MacErrors.h"
#include "MacTypes.h"
#include "OpenTransport.h"
#include "Threads.h"
#include "internal.hpp"
#include "sys/socket.h"
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <string>

extern "C" ssize_t recv(int fd, void *buf, size_t nbytes, int flags) {
  OTMemzero(buf, nbytes);

  // Get the Socket class that corresponds to this fd.
  auto s = openSockets.at(fd);

  printf(">%8ld...", nbytes);

  s->nbytes = nbytes; // Tell our socket to accept nbytes.

  // Initialize other variables.
  OTResult OSRcvResult = 0;
  size_t total = 0;
  OTFlags ot_flags = 0;

  YieldToAnyThread();

  // The socket's endpoint can either be in "asychronous mode", or "synchronous
  // mode". We set it to asynchronous mode in the socket function.

  // But if for any reason we're in synchronous mode and we want to add
  // up/OSRcvResulturn the amount of bytes we got.
  if (OTIsSynchronous(s->endpoint)) {
    OSErr OTRcvError = OTRcv(s->endpoint, buf, nbytes, &ot_flags);
    ThrowOSErr(OTRcvError);

    YieldToAnyThread();

    if (nbytes <= 64) {
      printf("%s\n", std::string((char *)buf, (char *)buf + nbytes).c_str());
      YieldToAnyThread();
    } else {
      printf("\n");
      YieldToAnyThread();
    }

    YieldToAnyThread();

    return OTRcvError;
    // in asynchronous mode we're gonna send one OTRcv and wait for it to
    // complete
  } else {
    // anything currently in the recieving buffer should just be return.
    bool fullBreak = true;
    if (s->recvBuf.size() > 0) {
      printf("seeing %ld leftover, hopefully getting %ld\n", s->recvBuf.size(),
             nbytes);
      goto recvWrite;
    }

    // s->recv_halt = true;

    OSRcvResult = OTRcv(s->endpoint, buf, nbytes, &ot_flags);

    if (OSRcvResult != kOTNoDataErr) {
      ThrowOSErr(OSRcvResult);
    }

    if (OSRcvResult >= 0) {
      printf("wanted %ld byte(s). wrote %c\n", nbytes, *(char *)buf);
      return OSRcvResult;
    }

    while (s->recv_halt) {
      YieldToAnyThread();
    }

  recvWrite:
    size_t offset = 0;
    size_t what = 0;

    auto remaining = nbytes;
    while (remaining != 0) {
      for (auto recvItem = s->recvBuf.begin(); recvItem != s->recvBuf.end();
           recvItem++) {

        if (recvItem->size() == 0) {
          continue;
        }

        while (remaining != 0 && recvItem->size() != 0) {
          auto fucker = recvItem->begin();
          printf("%c\n", *fucker);
          ((char *)buf)[offset] = *fucker;
          remaining--;
          offset++;
          if (remaining == 0) {
            fullBreak = true;
            break;
          }
          recvItem->erase(recvItem->begin(), recvItem->begin() + 1);
          YieldToAnyThread();
        }

        if (fullBreak) {
          break;
        }

        what++;
      }
      YieldToAnyThread();
    }

    return offset;
  }
}
