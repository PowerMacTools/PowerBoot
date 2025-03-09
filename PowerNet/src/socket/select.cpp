#include "sys/select.h"
#include "MacTypes.h"
#include "Threads.h"
#include "Timer.h"
#include "internal.hpp"
#include <cstdint>

extern "C" int select(int nfds, fd_set *__restrict readfds,
                      fd_set *__restrict writefds, fd_set *__restrict exceptfds,
                      struct timeval *__restrict timeout) {
  if (readfds != NULL) {
    auto reads = openSockets.at(readfds->fds_bits[0]);
    while (reads->recv_halt) {
      YieldToAnyThread();
    }
  }
  if (writefds != NULL) {
    auto writes = openSockets.at(writefds->fds_bits[0]);
    while (writes->send_halt) {
      YieldToAnyThread();
    }
  }

  // if (exceptfds != NULL) {
  //   auto excepts = openSockets.at(exceptfds->fds_bits[0]);
  //   while (excepts->halting) {
  //     YieldToAnyThread();
  //   }
  // }

  return 0;
};