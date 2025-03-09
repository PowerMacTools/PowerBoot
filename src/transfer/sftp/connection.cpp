
#include "../../console/console.hpp"
#include "sftp.hpp"
#include <cstdio>
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <stdexcept>

#ifdef __RETRO__
#include "OSUtils.h"
#include "Threads.h"
#include "Timer.h"
#endif

void SFTP::connect(ConnectionOptions options) {
  auto args = ReadThreadArgs(options, this);

#ifdef __RETRO__
  OSErr err = NewThread(kCooperativeThread, ::read_thread, &args, 100000,
                        kCreateIfNeeded, NULL, &read_thread_id);

  if (err < 0) {
    printf("Failed to create connection thread: %d", err);
  }

#else
  ::read_thread(&args);
#endif
  while (read_thread_state != ReadThreadState::Open) {
#ifdef __RETRO__
    YieldToAnyThread();
#endif
  }
}

int SFTP::wait() {
  int rc;
  fd_set fd;
  fd_set *writefd = NULL;
  fd_set *readfd = NULL;
  int dir;

  FD_ZERO(&fd);

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
  FD_SET(sock, &fd);
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

  /* now make sure we wait in the correct direction */
  // dir = libssh2_session_block_directions(session);

  if (dir & LIBSSH2_SESSION_BLOCK_INBOUND)
    readfd = &fd;

  if (dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
    writefd = &fd;

  rc = select((int)(sock + 1), readfd, writefd, NULL, NULL);

  return rc;
}
