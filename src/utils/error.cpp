#include "error.hpp"
#include <string>

std::runtime_error formatted_error(const char *format, ...) {
  va_list arglist;

  va_start(arglist, format);

  char *buf = (char *)malloc(255);
  vsnprintf(buf, 255, format, arglist);

  va_end(arglist);

  return std::runtime_error(std::string(buf));
}