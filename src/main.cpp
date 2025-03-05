#include "console/Console.hpp"
#include "transfer/sftp/sftp.hpp"
#include "transfer/transfer.hpp"
#include "utils/error.hpp"
#include <cstdio>
#include <exception>
#include <optional>
#include <stdexcept>
#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("Starting PowerBoot...\n");
  try {
    TransferProtocol *sftp = new SFTP();
    auto opt = ConnectionOptions("65.108.208.56", 22, "gavin", "0987Navel4321",
                                 ".:id_rsa.pub", ".:id_rsa", "/home/gavin/");
    sftp->connect(opt);

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
  } catch (std::runtime_error &ex) {
    printf("Runtime Error: %s\n", ex.what());
  } catch (std::exception &ex) {
    printf("Unhandled Exception: %s\n", ex.what());
  }
#ifdef __RETRO__
  retro::Console::currentInstance->ReadLine();
#endif
}