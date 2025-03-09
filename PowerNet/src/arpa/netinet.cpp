
#include <cctype>
#include <cstddef>

extern "C" unsigned long inet_addr(const char *cp) {
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
