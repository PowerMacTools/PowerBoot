#include "sys/select.h"
#include "Threads.h"
#include "internal.hpp"

extern "C" int select(int nfds, fd_set *__restrict readfds,
                      fd_set *__restrict writefds, fd_set *__restrict exceptfds,
                      struct timeval *__restrict timeout) {
  if (readfds != NULL) {
    auto reads = openSockets.at(readfds->fds_bits[0]);
    while (reads->halting) {
      YieldToAnyThread();
    }
  }
  if (writefds != NULL) {
    auto writes = openSockets.at(writefds->fds_bits[0]);
    while (writes->halting) {
      YieldToAnyThread();
    }
  }
  if (exceptfds != NULL) {
    auto excepts = openSockets.at(exceptfds->fds_bits[0]);
    while (excepts->halting) {
      YieldToAnyThread();
    }
  }

  return 0;
};