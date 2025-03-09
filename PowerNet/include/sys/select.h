#ifndef _SYS_SELECT_H
#define _SYS_SELECT_H
#ifdef __cplusplus
extern "C" {
#endif

#define _SIGSET_NWORDS (1024 / (8 * sizeof(unsigned long int)))
typedef struct {
  unsigned long int __val[_SIGSET_NWORDS];
} sigset_t;

#define FD_SETSIZE 1024

typedef unsigned long fd_mask;

typedef struct {
  unsigned long fds_bits[FD_SETSIZE / 8 / sizeof(long)];
} fd_set;

#define FD_ZERO(s)                                                             \
  do {                                                                         \
    int __i;                                                                   \
    unsigned long *__b = (s)->fds_bits;                                        \
    for (__i = sizeof(fd_set) / sizeof(long); __i; __i--)                      \
      *__b++ = 0;                                                              \
  } while (0)
#define FD_SET(d, s)                                                           \
  ((s)->fds_bits[(d) / (8 * sizeof(long))] |=                                  \
   (1UL << ((d) % (8 * sizeof(long)))))
#define FD_CLR(d, s)                                                           \
  ((s)->fds_bits[(d) / (8 * sizeof(long))] &=                                  \
   ~(1UL << ((d) % (8 * sizeof(long)))))
#define FD_ISSET(d, s)                                                         \
  !!((s)->fds_bits[(d) / (8 * sizeof(long))] &                                 \
     (1UL << ((d) % (8 * sizeof(long)))))

int select(int, fd_set *__restrict, fd_set *__restrict, fd_set *__restrict,
           struct timeval *__restrict);
int pselect(int, fd_set *__restrict, fd_set *__restrict, fd_set *__restrict,
            const struct timespec *__restrict, const sigset_t *__restrict);

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#define NFDBITS (8 * (int)sizeof(long))
#endif

#ifdef __cplusplus
}
#endif
#endif
