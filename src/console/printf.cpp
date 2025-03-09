#ifdef __RETRO__
#include "Threads.h"
#include "Windows.h"
#include "console.hpp"
#include "sys/types.h"

extern "C" ssize_t _consolewrite(int fd, const void *buf, size_t count) {
  const char *b = (const char *)buf;

  auto str = std::string(b, b + count);

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