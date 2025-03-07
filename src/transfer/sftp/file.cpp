#include "../../console/console.hpp"
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

Attributes *SFTP::lstat(std::string path, Attributes *attr) {
  LIBSSH2_SFTP_ATTRIBUTES attrs = ATTRIBUTE_TRANSLATE(attr);

  RC_ERR_HANDLE("libssh2_sftp_lstat", this,
                libssh2_sftp_lstat(sftp_pointer, path.c_str(), &attrs));

  return new SFTPAttributes(attrs);
};
void SFTP::mkdir(std::string path, int mode) {
  RC_ERR_HANDLE("libssh2_sftp_mkdir", this,
                libssh2_sftp_mkdir(sftp_pointer, path.c_str(), mode));
};
FileHandle *SFTP::openfile(std::string filename, unsigned long flags,
                           long mode) {
  LIBSSH2_SFTP_HANDLE *handle = NULL;

  do {
    handle =
        libssh2_sftp_open_ex(sftp_pointer, filename.c_str(), filename.length(),
                             flags, mode, LIBSSH2_SFTP_OPENFILE);

    if (!handle) {
      if (libssh2_session_last_errno(session) == LIBSSH2_ERROR_EAGAIN) {
        this->wait(); /* now we wait */
      } else {
        error_throw("Unable to open directory: %s\n",
                    this->error_msg(rc)->c_str());
      }
    }
  } while (!handle);

  return new SFTPFileHandle(this, handle);
};

DirHandle *SFTP::opendir(std::string path) {
  LIBSSH2_SFTP_HANDLE *handle = NULL;

  do {
    handle = libssh2_sftp_open_ex(sftp_pointer, path.c_str(), path.length(), 0,
                                  0, LIBSSH2_SFTP_OPENDIR);

    if (!handle) {
      if (libssh2_session_last_errno(session) == LIBSSH2_ERROR_EAGAIN) {
        this->wait(); /* now we wait */
      } else {
        error_throw("Unable to open directory: %s\n",
                    this->error_msg(rc)->c_str());
      }
    }
  } while (!handle);

  return new SFTPDirHandle(this, handle);
};

void SFTP::rename(std::string sourcefile, std::string destfile) {
  RC_ERR_HANDLE(
      "libssh2_sftp_rename", this,
      libssh2_sftp_rename(sftp_pointer, sourcefile.c_str(), destfile.c_str()));
};

void SFTP::rmdir(std::string path) {
  RC_ERR_HANDLE("libssh2_sftp_rmdir", this,
                libssh2_sftp_rmdir(sftp_pointer, path.c_str()));
};
void SFTP::setstat(std::string path, Attributes *attrs) {
  auto attr = ATTRIBUTE_TRANSLATE(attrs);
  RC_ERR_HANDLE("libssh2_sftp_setstat", this,
                libssh2_sftp_setstat(sftp_pointer, path.c_str(), &attr));
};
void SFTP::shutdown() {
  RC_ERR_HANDLE("libssh2_sftp_shutdown", this,
                libssh2_sftp_shutdown(sftp_pointer));
};

Attributes *SFTP::stat(std::string path) {
  LIBSSH2_SFTP_ATTRIBUTES attrs = {0};
  RC_ERR_HANDLE("libssh2_sftp_stat", this,
                libssh2_sftp_stat(sftp_pointer, path.c_str(), &attrs));
  return new SFTPAttributes(attrs);
};
StatVFS *SFTP::statvfs(std::string path) {
  LIBSSH2_SFTP_STATVFS *st =
      (LIBSSH2_SFTP_STATVFS *)malloc(sizeof(LIBSSH2_SFTP_STATVFS));
  RC_ERR_HANDLE(
      "libssh2_sftp_statvfs", this,
      libssh2_sftp_statvfs(sftp_pointer, path.c_str(), path.length(), st));

  return new SFTPStatVFS(*st);
};

void SFTP::symlink(std::string path, std::string target) {
  RC_ERR_HANDLE("libssh2_sftp_symlink", this,
                libssh2_sftp_symlink_ex(sftp_pointer, path.c_str(),
                                        path.length(), target.data(),
                                        target.size(), LIBSSH2_SFTP_SYMLINK));
};
void SFTP::readlink(std::string path, std::string target) {
  RC_ERR_HANDLE("libssh2_sftp_read_link", this,
                libssh2_sftp_symlink_ex(sftp_pointer, path.c_str(),
                                        path.length(), target.data(),
                                        target.size(), LIBSSH2_SFTP_READLINK));
};
void SFTP::realpath(std::string path, std::string target) {
  RC_ERR_HANDLE("libssh2_sftp_symlink", this,
                libssh2_sftp_symlink_ex(sftp_pointer, path.c_str(),
                                        path.length(), target.data(),
                                        target.size(), LIBSSH2_SFTP_REALPATH));
};

void SFTP::unlink(std::string filename) {
  RC_ERR_HANDLE("libssh2_sftp_unlink", this,
                libssh2_sftp_unlink_ex(sftp_pointer, filename.c_str(),
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
  char _buffer[512];
  char _longentry[512];
  LIBSSH2_SFTP_ATTRIBUTES attrs;

  rc = LIBSSH2_ERROR_EAGAIN;

  RC_ERR_HANDLE("libssh2_sftp_readdir_ex", this->sftp,
                libssh2_sftp_readdir_ex(this->handle, _buffer, sizeof(_buffer),
                                        _longentry, sizeof(_longentry),
                                        &attrs));

  if (rc == 0) {
    return {};
  }
  auto buffer = std::string(_buffer, _buffer + 512);
  auto longentry = std::string(_longentry, _longentry + 512);

  return File(buffer, longentry, new SFTPAttributes(attrs));
};

void SFTPDirHandle::close() {
  RC_ERR_HANDLE("libssh2_sftp_closedir", this->sftp,
                libssh2_sftp_closedir(this->handle));
};
