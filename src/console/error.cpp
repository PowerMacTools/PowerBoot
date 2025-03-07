#include "Threads.h"
#include "console.hpp"
#include <string>

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

  void *what;
  DisposeThread(main_thread_id, what, false);
  YieldToAnyThread();
}