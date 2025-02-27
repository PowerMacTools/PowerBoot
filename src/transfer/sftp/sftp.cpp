#include "sftp.hpp"
#include "../../utils/error.hpp"
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <unistd.h>

SFTP::SFTP() {
  rc = libssh2_init(0);
  if (rc) {
    throw formatted_error("libssh2 initialization failed (%s)", rc);
  }

  /* Create a session instance */
  session = libssh2_session_init();
  if (!session) {
    throw formatted_error("Could not initialize SSH session:",
                          this->error_msg(rc)->c_str());
  }

  /* Since we have set non-blocking, tell libssh2 we are non-blocking */
  libssh2_session_set_blocking(session, 0);
}

SFTP::~SFTP() {
  libssh2_sftp_close(sftp_handle);
  libssh2_sftp_shutdown(sftp_session);
  if (session) {
    while (libssh2_session_disconnect(session, "Normal Shutdown") ==
           LIBSSH2_ERROR_EAGAIN)
      ;
    libssh2_session_free(session);
  }

  if (sock != LIBSSH2_INVALID_SOCKET) {
    ::shutdown(sock, 2);
    close(sock);
  }

  libssh2_exit();
}

std::optional<std::string> SFTP::error_msg(int err) {
  if (err == LIBSSH2_ERROR_SFTP_PROTOCOL) {
    char *st = (char *)malloc(255);
    int len = 0;
    libssh2_session_last_error(session, &st, &len, 0);
    return std::string(st);
  }

  switch (err) {
  case LIBSSH2_ERROR_NONE:
    return "LIBSSH2_ERROR_NONE";
  case LIBSSH2_ERROR_SOCKET_NONE:
    return "LIBSSH2_ERROR_SOCKET_NONE";
  case LIBSSH2_ERROR_BANNER_RECV:
    return "LIBSSH2_ERROR_BANNER_RECV";
  case LIBSSH2_ERROR_BANNER_SEND:
    return "LIBSSH2_ERROR_BANNER_SEND";
  case LIBSSH2_ERROR_INVALID_MAC:
    return "LIBSSH2_ERROR_INVALID_MAC";
  case LIBSSH2_ERROR_KEX_FAILURE:
    return "LIBSSH2_ERROR_KEX_FAILURE";
  case LIBSSH2_ERROR_ALLOC:
    return "LIBSSH2_ERROR_ALLOC";
  case LIBSSH2_ERROR_SOCKET_SEND:
    return "LIBSSH2_ERROR_SOCKET_SEND";
  case LIBSSH2_ERROR_KEY_EXCHANGE_FAILURE:
    return "LIBSSH2_ERROR_KEY_EXCHANGE_FAILURE";
  case LIBSSH2_ERROR_TIMEOUT:
    return "LIBSSH2_ERROR_TIMEOUT";
  case LIBSSH2_ERROR_HOSTKEY_INIT:
    return "LIBSSH2_ERROR_HOSTKEY_INIT";
  case LIBSSH2_ERROR_HOSTKEY_SIGN:
    return "LIBSSH2_ERROR_HOSTKEY_SIGN";
  case LIBSSH2_ERROR_DECRYPT:
    return "LIBSSH2_ERROR_DECRYPT";
  case LIBSSH2_ERROR_SOCKET_DISCONNECT:
    return "LIBSSH2_ERROR_SOCKET_DISCONNECT";
  case LIBSSH2_ERROR_PROTO:
    return "LIBSSH2_ERROR_PROTO";
  case LIBSSH2_ERROR_PASSWORD_EXPIRED:
    return "LIBSSH2_ERROR_PASSWORD_EXPIRED";
  case LIBSSH2_ERROR_FILE:
    return "LIBSSH2_ERROR_FILE";
  case LIBSSH2_ERROR_METHOD_NONE:
    return "LIBSSH2_ERROR_METHOD_NONE";
  case LIBSSH2_ERROR_AUTHENTICATION_FAILED:
    return "LIBSSH2_ERROR_AUTHENTICATION_FAILED";
  case LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED:
    return "LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED";
  case LIBSSH2_ERROR_CHANNEL_OUTOFORDER:
    return "LIBSSH2_ERROR_CHANNEL_OUTOFORDER";
  case LIBSSH2_ERROR_CHANNEL_FAILURE:
    return "LIBSSH2_ERROR_CHANNEL_FAILURE";
  case LIBSSH2_ERROR_CHANNEL_REQUEST_DENIED:
    return "LIBSSH2_ERROR_CHANNEL_REQUEST_DENIED";
  case LIBSSH2_ERROR_CHANNEL_UNKNOWN:
    return "LIBSSH2_ERROR_CHANNEL_UNKNOWN";
  case LIBSSH2_ERROR_CHANNEL_WINDOW_EXCEEDED:
    return "LIBSSH2_ERROR_CHANNEL_WINDOW_EXCEEDED";
  case LIBSSH2_ERROR_CHANNEL_PACKET_EXCEEDED:
    return "LIBSSH2_ERROR_CHANNEL_PACKET_EXCEEDED";
  case LIBSSH2_ERROR_CHANNEL_CLOSED:
    return "LIBSSH2_ERROR_CHANNEL_CLOSED";
  case LIBSSH2_ERROR_CHANNEL_EOF_SENT:
    return "LIBSSH2_ERROR_CHANNEL_EOF_SENT";
  case LIBSSH2_ERROR_SCP_PROTOCOL:
    return "LIBSSH2_ERROR_SCP_PROTOCOL";
  case LIBSSH2_ERROR_ZLIB:
    return "LIBSSH2_ERROR_ZLIB";
  case LIBSSH2_ERROR_SOCKET_TIMEOUT:
    return "LIBSSH2_ERROR_SOCKET_TIMEOUT";
  case LIBSSH2_ERROR_SFTP_PROTOCOL:
    return "LIBSSH2_ERROR_SFTP_PROTOCOL";
  case LIBSSH2_ERROR_REQUEST_DENIED:
    return "LIBSSH2_ERROR_REQUEST_DENIED";
  case LIBSSH2_ERROR_METHOD_NOT_SUPPORTED:
    return "LIBSSH2_ERROR_METHOD_NOT_SUPPORTED";
  case LIBSSH2_ERROR_INVAL:
    return "LIBSSH2_ERROR_INVAL";
  case LIBSSH2_ERROR_INVALID_POLL_TYPE:
    return "LIBSSH2_ERROR_INVALID_POLL_TYPE";
  case LIBSSH2_ERROR_PUBLICKEY_PROTOCOL:
    return "LIBSSH2_ERROR_PUBLICKEY_PROTOCOL";
  case LIBSSH2_ERROR_EAGAIN:
    return "LIBSSH2_ERROR_EAGAIN";
  case LIBSSH2_ERROR_BUFFER_TOO_SMALL:
    return "LIBSSH2_ERROR_BUFFER_TOO_SMALL";
  case LIBSSH2_ERROR_BAD_USE:
    return "LIBSSH2_ERROR_BAD_USE";
  case LIBSSH2_ERROR_COMPRESS:
    return "LIBSSH2_ERROR_COMPRESS";
  case LIBSSH2_ERROR_OUT_OF_BOUNDARY:
    return "LIBSSH2_ERROR_OUT_OF_BOUNDARY";
  case LIBSSH2_ERROR_AGENT_PROTOCOL:
    return "LIBSSH2_ERROR_AGENT_PROTOCOL";
  case LIBSSH2_ERROR_SOCKET_RECV:
    return "LIBSSH2_ERROR_SOCKET_RECV";
  case LIBSSH2_ERROR_ENCRYPT:
    return "LIBSSH2_ERROR_ENCRYPT";
  case LIBSSH2_ERROR_BAD_SOCKET:
    return "LIBSSH2_ERROR_BAD_SOCKET";
  case LIBSSH2_ERROR_KNOWN_HOSTS:
    return "LIBSSH2_ERROR_KNOWN_HOSTS";
  case LIBSSH2_ERROR_CHANNEL_WINDOW_FULL:
    return "LIBSSH2_ERROR_CHANNEL_WINDOW_FULL";
  case LIBSSH2_ERROR_KEYFILE_AUTH_FAILED:
    return "LIBSSH2_ERROR_KEYFILE_AUTH_FAILED";
  case LIBSSH2_ERROR_RANDGEN:
    return "LIBSSH2_ERROR_RANDGEN";
  case LIBSSH2_ERROR_MISSING_USERAUTH_BANNER:
    return "LIBSSH2_ERROR_MISSING_USERAUTH_BANNER";
  case LIBSSH2_ERROR_ALGO_UNSUPPORTED:
    return "LIBSSH2_ERROR_ALGO_UNSUPPORTED";
  default:
    return {};
  };
}

int SFTP::wait() {
  struct timeval timeout;
  int rc;
  fd_set fd;
  fd_set *writefd = NULL;
  fd_set *readfd = NULL;
  int dir;

  timeout.tv_sec = 10;
  timeout.tv_usec = 0;

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
  dir = libssh2_session_block_directions(session);

  if (dir & LIBSSH2_SESSION_BLOCK_INBOUND)
    readfd = &fd;

  if (dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
    writefd = &fd;

  rc = select((int)(sock + 1), readfd, writefd, NULL, &timeout);

  return rc;
}

SFTPAttributes::SFTPAttributes(LIBSSH2_SFTP_ATTRIBUTES _attr) {
  this->attr = _attr;
};

unsigned long SFTPAttributes::flags() { return this->attr.flags; };
uint64_t SFTPAttributes::filesize() { return this->attr.filesize; };
unsigned long SFTPAttributes::uid() { return this->attr.uid; };
unsigned long SFTPAttributes::gid() { return this->attr.gid; };
unsigned long SFTPAttributes::permissions() { return this->attr.permissions; };
unsigned long SFTPAttributes::atime() { return this->attr.atime; };
unsigned long SFTPAttributes::mtime() { return this->attr.mtime; };

SFTPStatVFS::SFTPStatVFS(LIBSSH2_SFTP_STATVFS _vfs) { this->vfs = _vfs; }

uint64_t SFTPStatVFS::bsize() { return this->vfs.f_bsize; };
uint64_t SFTPStatVFS::frsize() { return this->vfs.f_frsize; };
uint64_t SFTPStatVFS::blocks() { return this->vfs.f_blocks; };
uint64_t SFTPStatVFS::bfree() { return this->vfs.f_bfree; };
uint64_t SFTPStatVFS::bavail() { return this->vfs.f_bavail; };
uint64_t SFTPStatVFS::files() { return this->vfs.f_files; };
uint64_t SFTPStatVFS::ffree() { return this->vfs.f_ffree; };
uint64_t SFTPStatVFS::favail() { return this->vfs.f_favail; };
uint64_t SFTPStatVFS::fsid() { return this->vfs.f_fsid; };
uint64_t SFTPStatVFS::flag() { return this->vfs.f_flag; };
uint64_t SFTPStatVFS::namemax() { return this->vfs.f_namemax; };