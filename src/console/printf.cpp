#include "Threads.h"
#include "Windows.h"
#include "console.hpp"
#include <sys/types.h>

extern "C" ssize_t _consolewrite(int fd, const void *buf, size_t count) {
  const char *b = (const char *)buf;
  auto str = std::string(b, b + count);

  ThreadBeginCritical();
  lineBuffer.push_back(str);
  ThreadEndCritical();

  // BeginUpdate(window);
  // ScreenDraw(window->visRgn);
  // EndUpdate(window);
  return str.size();
}

extern "C" ssize_t _consoleread(int fd, void *buf, size_t count) { return 0; }