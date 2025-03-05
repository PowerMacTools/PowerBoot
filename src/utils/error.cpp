#include "error.hpp"
#include "console/Console.hpp"
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
#ifdef __RETRO__
  if (!retro::Console::currentInstance) {
    retro::InitConsole();
  }
  retro::Console::currentInstance->ReadLine();
#endif
  exit(0);
}