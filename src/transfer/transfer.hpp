#ifndef __TRANSFER_PROTOCOL_HPP
#define __TRANSFER_PROTOCOL_HPP

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

/**
  Attributes of a file.
*/
class Attributes {
public:
  virtual unsigned long flags() = 0;
  virtual uint64_t filesize() = 0;
  virtual unsigned long uid() = 0;
  virtual unsigned long gid() = 0;
  virtual unsigned long permissions() = 0;
  virtual unsigned long atime() = 0;
  virtual unsigned long mtime() = 0;
};

/**
  Attributes of a mounted filesystem.
*/
class StatVFS {
public:
  virtual uint64_t bsize() = 0;   /* file system block size */
  virtual uint64_t frsize() = 0;  /* fragment size */
  virtual uint64_t blocks() = 0;  /* size of fs in frsize units */
  virtual uint64_t bfree() = 0;   /* # free blocks */
  virtual uint64_t bavail() = 0;  /* # free blocks for non-root */
  virtual uint64_t files() = 0;   /* # inodes */
  virtual uint64_t ffree() = 0;   /* # free inodes */
  virtual uint64_t favail() = 0;  /* # free inodes for non-root */
  virtual uint64_t fsid() = 0;    /* file system ID */
  virtual uint64_t flag() = 0;    /* mount flags */
  virtual uint64_t namemax() = 0; /* maximum filename length */
};

/**
  Handle to a file returned by the transfer protocol
*/
class FileHandle {
  virtual std::vector<int8_t> read() = 0;
  virtual void rewind() = 0;
  virtual void seek(size_t offset) = 0;
  virtual uint64_t tell() = 0;
  virtual void write(std::vector<int8_t> buf) = 0;
  virtual Attributes *stat() = 0;
  virtual void setstat(Attributes *attr) = 0;
  virtual void close() = 0;
  virtual void sync() = 0;
  virtual StatVFS *statvfs() = 0;
};

/**
  Read-only file data returned by DirIterator.
*/
class File {
private:
  std::vector<int8_t> _buffer;
  std::string _longentry;
  Attributes *_attributes;

public:
  File(std::vector<int8_t> buffer, std::string longentry,
       Attributes *attributes)
      : _buffer(buffer), _longentry(longentry), _attributes(attributes) {};
  std::vector<int8_t> *buffer() { return &this->_buffer; };
  std::string *longentry() { return &this->_longentry; };
  Attributes *attributes() { return this->_attributes; };
};

/**
  Handle to a directory. Use next() to read each file within it.
*/
class DirHandle {
public:
  virtual std::optional<File> next() = 0;
  virtual void close() = 0;
};

class TransferProtocol {
private:
public:
  virtual void connect(std::string addr, uint16_t port, std::string username,
                       std::string password, std::string pubkey,
                       std::string privkey, std::string path) = 0;
  virtual Attributes *lstat(std::string path, Attributes *attributes) = 0;
  virtual void mkdir(std::string path, int mode) = 0;

  virtual FileHandle *openfile(std::string filename, unsigned long flags,
                               long mode) = 0;

  virtual DirHandle *opendir(std::string filename) = 0;

  virtual void rename(std::string sourcefile, std::string destfile) = 0;
  virtual void rmdir(std::string path) = 0;
  virtual void setstat(std::string path, Attributes *attrs) = 0;
  virtual void shutdown() = 0;
  virtual Attributes *stat(std::string path) = 0;
  virtual StatVFS *statvfs(std::string path) = 0;

  virtual void symlink(std::string path, std::string target) = 0;
  virtual void readlink(std::string path, std::string target) = 0;
  virtual void realpath(std::string path, std::string target) = 0;
  virtual void unlink(std::string filename) = 0;
};

#endif