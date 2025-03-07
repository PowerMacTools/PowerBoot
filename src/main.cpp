#include "console/console.hpp"
#include "transfer/sftp/sftp.hpp"
#include "transfer/transfer.hpp"
#include <cstddef>
#include <cstdio>
#include <exception>
#include <optional>
#include <stdexcept>
#include <stdio.h>

#define __RETRO__

#ifdef __RETRO__
#include "MacTypes.h"
#include "Memory.h"
#include "Threads.h"
ThreadID main_thread_id;
void MacMain();
#else
#define YieldToAnyThread()
#endif

void *main_func(void *arg);

int main(int argc, char *argv[]) {
  // On Classic Mac we want to start the main function on a new thread.

#ifdef __RETRO__
  MaxApplZone();

  for (int i = 0; i < 10; i++) {
    MoreMasters();
  }

  OSErr err = NewThread(kCooperativeThread, main_func, NULL, 100000,
                        kCreateIfNeeded, NULL, &main_thread_id);

  if (err < 0) {
    printf("Failed to create main thread: %d", err);
  }

  MacMain();
#else
  main_func(NULL);
#endif
}

void *main_func(void *arg) {
  printf("Starting PowerBoot...\n");
  YieldToAnyThread();
  try {
    TransferProtocol *sftp = new SFTP();
    YieldToAnyThread();
    auto opt = ConnectionOptions("192.168.1.145", 22, "gavin", "UnitedSenora02",
                                 "/home/gavin/.ssh/id_rsa.pub",
                                 "/home/gavin/.ssh/id_rsa", "/home/gavin/");
    YieldToAnyThread();
    sftp->connect(opt);
    YieldToAnyThread();

    auto dir = sftp->opendir(".");
    YieldToAnyThread();

    std::optional<File> file = dir->next();
    YieldToAnyThread();

    for (; file.has_value(); file = dir->next()) {
      auto val = file.value();
      auto filename = *val.buffer();

      Attributes *attr = sftp->stat(filename);
      YieldToAnyThread();
      if (attr == NULL) {
        printf("%s\n", filename.c_str());
      } else {
        printf("[%ldKB] %s\n", attr->filesize(), filename.c_str());
      }
      YieldToAnyThread();
    }

    dir->close();
    YieldToAnyThread();
  } catch (std::runtime_error &ex) {
    printf("Runtime Error: %s\n", ex.what());
  } catch (std::exception &ex) {
    printf("Unhandled Exception: %s\n", ex.what());
  }
  return NULL;
}
