#include "../../console/console.hpp"

#include "sftp.hpp"
#include <cerrno>
#include <unistd.h>

#ifdef __RETRO__
#include "unixnet2mac.h"
#endif

ssize_t SFTP::recv_callback(libssh2_socket_t sockfd, void *buffer,
                            size_t length, int flags, void **abstract) {
  int rc = recv(sockfd, buffer, length, flags);
  printf("read: %d\n", rc);

  if (rc < 0) {
    if (rc == -11 || rc == -1) {
      return -EAGAIN;
    } else {
      read_error_throw("read error: %d\n", rc);
    }
  }
  return rc;
}

ssize_t SFTP::send_callback(libssh2_socket_t sockfd, const void *buffer,
                            size_t length, int flags, void **abstract) {
  int rc = 0;
  rc = send(sockfd, buffer, length, flags);
  printf("written: %d\n", rc);

  if (rc <= 0) {
    if (rc == -11 || rc == -1) {
      return -EAGAIN;
    } else {
      read_error_throw("write error: %d\n", rc);
    }
  }
  return rc;
}
