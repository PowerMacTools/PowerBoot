#include "console/Console.hpp"
#include "transfer/sftp/sftp.hpp"
#include "transfer/transfer.hpp"
#include "utils/error.hpp"
#include <cstdio>
#include <exception>
#include <optional>
#include <stdexcept>
#include <stdio.h>

#ifdef __RETRO__
#include "MacTypes.h"
#include "Threads.h"
ThreadID main_thread_id;
#endif

void *main_func(void *arg);

int main(int argc, char *argv[]) {
  // // On Classic Mac we want to start the main function on a new thread.
  // OSErr err = NewThread(kCooperativeThread, main_func, NULL, 100000,
  //                       kCreateIfNeeded, NULL, &main_thread_id);

  // if (err < 0) {
  //   printf("Failed to create main thread: %d", err);
  // }
  main_func(NULL);
}

void *main_func(void *arg) {
  printf("Starting PowerBoot...\n");
  try {
    TransferProtocol *sftp = new SFTP();
    auto opt = ConnectionOptions("192.168.1.145", 22, "gavin", "UnitedSenora02",
                                 "/home/gavin/.ssh/id_rsa.pub",
                                 "/home/gavin/.ssh/id_rsa", "/home/gavin/");
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