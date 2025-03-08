#include "console.hpp"
#include <cstdlib>
#include <string>

#ifdef __RETRO__
#include "Threads.h"
#endif

std::runtime_error formatted_error(const char *format, ...) {
  va_list arglist;

  va_start(arglist, format);

  char *buf = (char *)malloc(255);
  vsnprintf(buf, 255, format, arglist);

  va_end(arglist);

  return std::runtime_error(std::string(buf));
}

void error_throw(const char *format, ...) {
  va_list arglist;

  va_start(arglist, format);

  char *buf = (char *)malloc(255);
  vsnprintf(buf, 255, format, arglist);

  va_end(arglist);

  printf("%s\n", buf);

#ifdef __RETRO__
  void *what;
  DisposeThread(main_thread_id, what, false);
  YieldToAnyThread();
#else
  exit(0);
#endif
}

void read_error_throw(const char *format, ...) {
  va_list arglist;

  va_start(arglist, format);

  char *buf = (char *)malloc(255);
  vsnprintf(buf, 255, format, arglist);

  va_end(arglist);

  printf("%s\n", buf);

#ifdef __RETRO__
  void *what;
  DisposeThread(read_thread_id, what, false);
  YieldToAnyThread();
#else
  exit(0);
#endif
}