#include "../transfer.hpp"
#include <cstdint>

#include <optional>
#include <stdio.h>
#include <string>
#include <vector>

#include <libssh2.h>
#include <libssh2_sftp.h>

#ifdef __RETRO__
#include <unixnet2mac.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#ifndef __SFTP_HPP
#define __SFTP_HPP

#define RC_ERR_HANDLE(funcname, th, cmd)                                       \
  rc = LIBSSH2_ERROR_EAGAIN;                                                   \
  while (rc == LIBSSH2_ERROR_EAGAIN) {                                         \
    rc = cmd;                                                                  \
    if (rc == LIBSSH2_ERROR_EAGAIN) {                                          \
      th->wait();                                                              \
    }                                                                          \
  }                                                                            \
  if (rc < 0) {                                                                \
    error_throw("[" funcname "] %s", th->error_msg(rc)->c_str());              \
  }

class SFTP;

class SFTPAttributes : public Attributes {
private:
  LIBSSH2_SFTP_ATTRIBUTES attr;

public:
  SFTPAttributes(LIBSSH2_SFTP_ATTRIBUTES attr);
  unsigned long flags() override;
  uint64_t filesize() override;
  unsigned long uid() override;
  unsigned long gid() override;
  unsigned long permissions() override;
  unsigned long atime() override;
  unsigned long mtime() override;
};

class SFTPStatVFS : public StatVFS {
private:
  LIBSSH2_SFTP_STATVFS vfs;

public:
  SFTPStatVFS(LIBSSH2_SFTP_STATVFS vfs);
  uint64_t bsize() override;
  uint64_t frsize() override;
  uint64_t blocks() override;
  uint64_t bfree() override;
  uint64_t bavail() override;
  uint64_t files() override;
  uint64_t ffree() override;
  uint64_t favail() override;
  uint64_t fsid() override;
  uint64_t flag() override;
  uint64_t namemax() override;
};

class SFTPFileHandle : public FileHandle {
  LIBSSH2_SFTP_HANDLE *handle;
  SFTP *sftp;
  int rc;

public:
  SFTPFileHandle(SFTP *sftp, LIBSSH2_SFTP_HANDLE *handle)
      : sftp(sftp), handle(handle) {};
  std::vector<int8_t> read() override;
  void rewind() override;
  void seek(size_t offset) override;
  uint64_t tell() override;
  void write(std::vector<int8_t> buf) override;

  Attributes *stat() override;
  void close() override;

  void setstat(Attributes *attr) override;
  StatVFS *statvfs() override;
  void sync() override;
};

class SFTPDirHandle : public DirHandle {
  LIBSSH2_SFTP_HANDLE *handle;
  SFTP *sftp;
  int rc;

public:
  SFTPDirHandle(SFTP *sftp, LIBSSH2_SFTP_HANDLE *handle)
      : sftp(sftp), handle(handle) {};
  std::optional<File> next() override;
  void close() override;
};

class SFTP : public TransferProtocol {
private:
  uint32_t hostaddr;
  libssh2_socket_t sock;
  int i = 0;
  int auth_pw = 0;

  const char *fingerprint;
  int rc;
  LIBSSH2_SFTP *sftp_session;
  LIBSSH2_SFTP_HANDLE *sftp_handle;
  char *userauthlist;

  libssh2_struct_stat_size total = 0;
  int spin = 0;

  struct sockaddr_in sin;

public:
  LIBSSH2_SESSION *session = NULL;

  SFTP();
  ~SFTP();

  std::optional<std::string> error_msg();
  std::optional<std::string> error_msg(int err);
  int wait();

  void connect(ConnectionOptions options) override;

  Attributes *lstat(std::string path, Attributes *attributes) override;
  void mkdir(std::string path, int mode) override;

  FileHandle *openfile(std::string filename, unsigned long flags,
                       long mode) override;
  DirHandle *opendir(std::string filename) override;

  void rename(std::string sourcefile, std::string destfile) override;
  void rmdir(std::string path) override;
  void setstat(std::string path, Attributes *attrs) override;
  void shutdown() override;
  Attributes *stat(std::string path) override;
  StatVFS *statvfs(std::string path) override;
  void symlink(std::string path, std::string target) override;

  void readlink(std::string path, std::string target) override;
  void realpath(std::string path, std::string target) override;

  void unlink(std::string filename) override;

  static ssize_t recv_callback(libssh2_socket_t sockfd, void *buffer,
                               size_t length, int flags, void **abstract);
  static ssize_t send_callback(libssh2_socket_t sockfd, const void *buffer,
                               size_t length, int flags, void **abstract);
  friend class SFTPAttributes;
};

#endif