#include "OpenTransport.h"
#include "OpenTransportProviders.h"
#include "Threads.h"
#include "internal.hpp"
#include "unixnet2mac.h"
#include <cstdarg>
#include <cstdlib>
#include <ctype.h>

extern ThreadID main_thread_id;

unsigned long inet_addr(const char *cp) {
  unsigned long val;
  int base, n;
  char c;
  unsigned long parts[4];
  int i;

  if (cp == NULL || *cp == '\0') {
    return 0xFFFFFFFF;
  }

  for (i = 0; i < 4; i++) {
    val = 0;
    base = 10;
    if (*cp == '0') {
      if (*++cp == 'x' || *cp == 'X') {
        base = 16;
        cp++;
      } else {
        base = 8;
      }
    }

    while ((c = *cp) != '\0') {
      if (isdigit(c)) {
        n = c - '0';
      } else if (base == 16 && isxdigit(c)) {
        n = tolower(c) - 'a' + 10;
      } else {
        break;
      }
      if (n >= base) {
        return 0xFFFFFFFF; // Invalid character for base
      }
      val = (val * base) + n;
      cp++;
    }
    if (*cp == '.') {
      if (i == 3) {
        return 0xFFFFFFFF; // Too many dots
      }
      cp++;
    } else if (*cp != '\0') {
      return 0xFFFFFFFF; // Invalid character after number
    }
    parts[i] = val;
  }

  if (i != 4) {
    return 0xFFFFFFFF; // Not enough parts
  }

  return (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3];
}

Socket::~Socket() { OTCloseProvider(endpoint); }

std::unordered_map<size_t, Socket *> openSockets =
    std::unordered_map<size_t, Socket *>();

void mac_error_throw(const char *format, ...) {
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
