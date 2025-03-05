#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#ifndef __ERROR_HPP
#define __ERROR_HPP

std::runtime_error formatted_error(const char *format, ...);
void error_throw(const char *format, ...);

#endif