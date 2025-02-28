#include "transfer/sftp/sftp.hpp"
#include "transfer/transfer.hpp"
#include <cstdio>
#include <optional>
#include <stdexcept>
#include <stdio.h>

int main(int argc, char *argv[]) {
  try {
    TransferProtocol *sftp = new SFTP();
    sftp->connect("192.168.1.145", 22, "gavin", "UnitedSenora02",
                  "/home/gavin/.ssh/id_rsa.pub", "/home/gavin/.ssh/id_rsa",
                  "/home/gavin/");

    auto dir = sftp->opendir(".");

    std::optional<File> file = dir->next();

    for (; file.has_value(); file = dir->next()) {
      auto val = file.value();
      auto filename = *val.buffer();

      Attributes *attr = sftp->stat(filename);
      if (attr == NULL) {
        printf("%s\n", filename.c_str());
      } else {
        printf("[%ldKB] %s\n", attr->filesize(), filename.c_str());
      }
    }

    dir->close();
  } catch (std::runtime_error *ex) {
    printf("%s\n", ex->what());
  } catch (std::runtime_error ex) {
    printf("%s\n", ex.what());
  }
}