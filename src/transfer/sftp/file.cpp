#include "../../utils/error.hpp"
#include "sftp.hpp"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <string>
#include <vector>

#define ATTRIBUTE_TRANSLATE(attr)                                              \
  LIBSSH2_SFTP_ATTRIBUTES {                                                    \
    .flags = attr->flags(), .filesize = attr->filesize(), .uid = attr->uid(),  \
    .gid = attr->gid(), .permissions = attr->permissions(),                    \
    .atime = attr->atime(), .mtime = attr->mtime(),                            \
  }

#define RC_ERR_HANDLE(funcname, th, cmd)                                       \
  rc = LIBSSH2_ERROR_EAGAIN;                                                   \
  while (rc == LIBSSH2_ERROR_EAGAIN) {                                         \
    rc = cmd;                                                                  \
    if (rc == LIBSSH2_ERROR_EAGAIN) {                                          \
      th->wait();                                                              \
    }                                                                          \
  }                                                                            \
  if (rc < 0) {                                                                \
    throw formatted_error("[" funcname "] %s", th->error_msg(rc)->c_str());    \
  };

Attributes *SFTP::lstat(std::string path, Attributes *attr) {
  LIBSSH2_SFTP_ATTRIBUTES attrs = ATTRIBUTE_TRANSLATE(attr);

  RC_ERR_HANDLE("libssh2_sftp_lstat", this,
                libssh2_sftp_lstat(sftp_session, path.c_str(), &attrs));

  return new SFTPAttributes(attrs);
};
void SFTP::mkdir(std::string path, int mode) {
  RC_ERR_HANDLE("libssh2_sftp_lstat", this,
                libssh2_sftp_mkdir(sftp_session, path.c_str(), mode));
};
FileHandle *SFTP::openfile(std::string filename, unsigned long flags,
                           long mode) {
  LIBSSH2_SFTP_HANDLE *handle = NULL;

  do {
    handle =
        libssh2_sftp_open_ex(sftp_session, filename.c_str(), filename.length(),
                             flags, mode, LIBSSH2_SFTP_OPENFILE);

    if (!handle) {
      if (libssh2_session_last_errno(session) == LIBSSH2_ERROR_EAGAIN) {
        this->wait(); /* now we wait */
      } else {
        throw formatted_error("Unable to open directory: %s\n",
                              this->error_msg(rc)->c_str());
      }
    }
  } while (!handle);

  return new SFTPFileHandle(this, handle);
};

DirHandle *SFTP::opendir(std::string path) {
  LIBSSH2_SFTP_HANDLE *handle = NULL;

  do {
    handle = libssh2_sftp_open_ex(sftp_session, path.c_str(), path.length(), 0,
                                  0, LIBSSH2_SFTP_OPENDIR);

    if (!handle) {
      if (libssh2_session_last_errno(session) == LIBSSH2_ERROR_EAGAIN) {
        this->wait(); /* now we wait */
      } else {
        throw formatted_error("Unable to open directory: %s\n",
                              this->error_msg(rc)->c_str());
      }
    }
  } while (!handle);

  return new SFTPDirHandle(this, handle);
};

void SFTP::rename(std::string sourcefile, std::string destfile) {
  RC_ERR_HANDLE(
      "libssh2_sftp_rename", this,
      libssh2_sftp_rename(sftp_session, sourcefile.c_str(), destfile.c_str()));
};

void SFTP::rmdir(std::string path) {
  RC_ERR_HANDLE("libssh2_sftp_rmdir", this,
                libssh2_sftp_rmdir(sftp_session, path.c_str()));
};
void SFTP::setstat(std::string path, Attributes *attrs) {
  auto attr = ATTRIBUTE_TRANSLATE(attrs);
  RC_ERR_HANDLE("libssh2_sftp_setstat", this,
                libssh2_sftp_setstat(sftp_session, path.c_str(), &attr));
};
void SFTP::shutdown() {
  RC_ERR_HANDLE("libssh2_sftp_shutdown", this,
                libssh2_sftp_shutdown(sftp_session));
};

Attributes *SFTP::stat(std::string path) {
  LIBSSH2_SFTP_ATTRIBUTES attrs = {0};
  RC_ERR_HANDLE("libssh2_sftp_stat", this,
                libssh2_sftp_stat(sftp_session, path.c_str(), &attrs));
  return new SFTPAttributes(attrs);
};
StatVFS *SFTP::statvfs(std::string path) {
  LIBSSH2_SFTP_STATVFS *st =
      (LIBSSH2_SFTP_STATVFS *)malloc(sizeof(LIBSSH2_SFTP_STATVFS));
  RC_ERR_HANDLE(
      "libssh2_sftp_statvfs", this,
      libssh2_sftp_statvfs(sftp_session, path.c_str(), path.length(), st));

  return new SFTPStatVFS(*st);
};

void SFTP::symlink(std::string path, std::string target) {
  RC_ERR_HANDLE("libssh2_sftp_symlink", this,
                libssh2_sftp_symlink_ex(sftp_session, path.c_str(),
                                        path.length(), target.data(),
                                        target.size(), LIBSSH2_SFTP_SYMLINK));
};
void SFTP::readlink(std::string path, std::string target) {
  RC_ERR_HANDLE("libssh2_sftp_read_link", this,
                libssh2_sftp_symlink_ex(sftp_session, path.c_str(),
                                        path.length(), target.data(),
                                        target.size(), LIBSSH2_SFTP_READLINK));
};
void SFTP::realpath(std::string path, std::string target) {
  RC_ERR_HANDLE("libssh2_sftp_symlink", this,
                libssh2_sftp_symlink_ex(sftp_session, path.c_str(),
                                        path.length(), target.data(),
                                        target.size(), LIBSSH2_SFTP_REALPATH));
};

void SFTP::unlink(std::string filename) {
  RC_ERR_HANDLE("libssh2_sftp_unlink", this,
                libssh2_sftp_unlink_ex(sftp_session, filename.c_str(),
                                       filename.length()));
};

void SFTPFileHandle::rewind() { libssh2_sftp_rewind(this->handle); };
void SFTPFileHandle::seek(size_t offset) {
  libssh2_sftp_seek(this->handle, offset);
};
uint64_t SFTPFileHandle::tell() { return libssh2_sftp_tell(this->handle); };

std::vector<int8_t> SFTPFileHandle::read() {
  auto stat = this->stat();

  auto filesize = stat->filesize();
  char *buffer = (char *)malloc(filesize);

  RC_ERR_HANDLE("libssh2_sftp_read", this->sftp,
                libssh2_sftp_read(this->handle, buffer, filesize));

  return std::vector<int8_t>(buffer, buffer + filesize);
};

void SFTPFileHandle::write(std::vector<int8_t> buf) {
  auto data = (char *)buf.data();

  RC_ERR_HANDLE("libssh2_sftp_write", this->sftp,
                libssh2_sftp_write(this->handle, data, buf.size()));
};

void SFTPFileHandle::close() {
  RC_ERR_HANDLE("libssh2_sftp_close", this->sftp,
                libssh2_sftp_close(this->handle));
};

void SFTPFileHandle::setstat(Attributes *attr) {
  LIBSSH2_SFTP_ATTRIBUTES attrs = ATTRIBUTE_TRANSLATE(attr);
  RC_ERR_HANDLE("libssh2_sftp_fsetstat", this->sftp,
                libssh2_sftp_fsetstat(this->handle, &attrs));
};
Attributes *SFTPFileHandle::stat() {
  LIBSSH2_SFTP_ATTRIBUTES *attrs =
      (LIBSSH2_SFTP_ATTRIBUTES *)malloc(sizeof(LIBSSH2_SFTP_ATTRIBUTES));
  RC_ERR_HANDLE("file/libssh2_sftp_fstat", this->sftp,
                libssh2_sftp_fstat(this->handle, attrs));

  return new SFTPAttributes(*attrs);
};
StatVFS *SFTPFileHandle::statvfs() {
  LIBSSH2_SFTP_STATVFS *vfs =
      (LIBSSH2_SFTP_STATVFS *)malloc(sizeof(LIBSSH2_SFTP_STATVFS));
  RC_ERR_HANDLE("libssh2_sftp_fstatvfs", this->sftp,
                libssh2_sftp_fstatvfs(this->handle, vfs));

  return new SFTPStatVFS(*vfs);
};

void SFTPFileHandle::sync() {
  RC_ERR_HANDLE("libssh2_sftp_fsync", this->sftp,
                libssh2_sftp_fsync(this->handle));
};

std::optional<File> SFTPDirHandle::next() {
  // Get the attributes of the current file so that we can get its length.
  LIBSSH2_SFTP_ATTRIBUTES attrs = {0};

  RC_ERR_HANDLE("dir/libssh2_sftp_fstat", this->sftp,
                libssh2_sftp_fstat(this->handle, &attrs));

  auto len = attrs.filesize;

  char *_buffer = (char *)malloc(len * 2);
  char *_longentry = (char *)malloc(1024);

  rc = LIBSSH2_ERROR_EAGAIN;

  RC_ERR_HANDLE("libssh2_sftp_readdir_ex", this->sftp,
                libssh2_sftp_readdir_ex(this->handle, _buffer, len, _longentry,
                                        255, &attrs));

  if (rc < 0 && rc != LIBSSH2_ERROR_EAGAIN) {
    auto err = this->sftp->error_msg(rc);
    if (err.has_value()) {

      throw formatted_error("[libssh2_sftp_readdir_ex] %s",
                            err.value().c_str());
    }
  }
  auto buffer = std::vector<int8_t>(_buffer, _buffer + len);
  auto longentry = std::string(_longentry, _longentry + 1024);

  return File(buffer, longentry, new SFTPAttributes(attrs));
};

void SFTPDirHandle::close() {
  RC_ERR_HANDLE("libssh2_sftp_closedir", this->sftp,
                libssh2_sftp_closedir(this->handle));
};
