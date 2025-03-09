#include <time.h>

struct timeval {
  __time_t tv_sec;       /* Seconds.  */
  __suseconds_t tv_usec; /* Microseconds.  */
};
