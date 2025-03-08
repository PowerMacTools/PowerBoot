
#include "sftp.hpp"
#include <cerrno>
#include <unistd.h>

#ifdef __RETRO__
#include "unixnet2mac.h"
#endif

ssize_t SFTP::recv_callback(libssh2_socket_t sockfd, void *buffer,
                            size_t length, int flags, void **abstract) {
  int r = EAGAIN;
  while (r != EAGAIN) {
    r = recv(sockfd, buffer, length, flags);
  };

  return r;
}

ssize_t SFTP::send_callback(libssh2_socket_t sockfd, const void *buffer,
                            size_t length, int flags, void **abstract) {
  return send(sockfd, buffer, length, flags);
}
