#ifdef __RETRO__
#include "Threads.h"
#include "Windows.h"
#include "console.hpp"
#include <sys/types.h>

extern "C" ssize_t _consolewrite(int fd, const void *buf, size_t count) {
  const char *b = (const char *)buf;

  // We manually construct the string so that if we run into a
  // buffer overrun error the entire system at least doesn't hang.
  auto str = std::string();
  for (size_t i = 0; i < count; i++) {
    str.push_back(b[i]);
    YieldToAnyThread();
  };
  // yes it's slower but this is for a debug window that the user might not even
  // have open so it's fine

  ThreadBeginCritical();
  lineBuffer.push_back(str);
  if (lineBuffer.size() >= 40) {
    lineBuffer.erase(lineBuffer.begin(), lineBuffer.begin() + 1);
  }
  ThreadEndCritical();

  YieldToAnyThread();

  return str.size();
}

extern "C" ssize_t _consoleread(int fd, void *buf, size_t count) { return 0; }
#endif