#include "sftp.hpp"
#include "../../utils/error.hpp"
#include <cstdio>
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <unistd.h>

SFTP::SFTP() {
  rc = libssh2_init(0);
  if (rc) {
    error_throw("libssh2 initialization failed (%s)", rc);
  }

  /* Create a session instance */
  session = libssh2_session_init();
  if (!session) {
    error_throw("Could not initialize SSH session:",
                this->error_msg(rc)->c_str());
  }

#ifdef __RETRO__
  libssh2_session_callback_set(session, LIBSSH2_CALLBACK_RECV,
                               (void *)SFTP::recv_callback);

  libssh2_session_callback_set(session, LIBSSH2_CALLBACK_RECV,
                               (void *)SFTP::send_callback);
#endif
  /* Since we have set non-blocking, tell libssh2 we are non-blocking */
  // libssh2_session_set_blocking(session, 0);
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
#ifdef __linux__
    ::shutdown(sock, 2);
#endif

    close(sock);
  }

  libssh2_exit();
}

std::optional<std::string> SFTP::error_msg() { return this->error_msg(0); }

std::optional<std::string> SFTP::error_msg(int err) {
  if (err == 1) {
    return {};
  }
  char *st = (char *)malloc(255);
  char *st2 = (char *)malloc(255);
  int len = 0;
  libssh2_session_last_error(session, &st, &len, 0);

  snprintf(st2, 255, " (%d)", err);
  return std::string(st) + std::string(st2);
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