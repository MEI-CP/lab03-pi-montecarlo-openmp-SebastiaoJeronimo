#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <limits.h>
#include <stdint.h>
#include <assert.h>
#include <omp.h>

int silent = 0;
int debug = 0;

#define DEBUG(statement) if(debug) statement

// Use 0 foi default rand(), 1, 2 or 3 for alternative generators
#define USE_MYRAND  4

/**
 * FROM: https://gist.github.com/diabloneo/9619917
 * @fn timespec_diff(struct timespec *, struct timespec *, struct timespec *)
 * @brief Compute the diff of two timespecs, that is a - b = result.
 * @param a the minuend
 * @param b the subtrahend
 * @param result a - b
 */
static inline void timespec_diff(struct timespec *a, struct timespec *b,
    struct timespec *result) {
    result->tv_sec  = a->tv_sec  - b->tv_sec;
    result->tv_nsec = a->tv_nsec - b->tv_nsec;
    if (result->tv_nsec < 0) {
        --result->tv_sec;
        result->tv_nsec += 1e9L;
    }
}

double timespec_to_milisecs (struct timespec *t) {
  double tf = (double)t->tv_sec + t->tv_nsec / 1e9L;
  return tf;
}

void usage () {
  printf ("Usage: approxPi [-s] [-d] nPoints nThreads\n\
                  -d: Enable debugging… more info is printed on the screen.\n\
                  -s: Silent mode… just print numbers with no explanation\n\
                      of their meaning.\n\
             nPoints: The total number of points to be used in the simulation\n\
                      (to be divided by the working threads).\n\
            nThreads: The number of working threads.");
  exit (1);
}

unsigned int myrand_r1 (unsigned int *seed) {
  *seed = (*seed >> 1) | (*seed << (sizeof(*seed)*8-1));
  return *seed;
}

unsigned int myrand_r2 (unsigned int *seed) {
  *seed += (unsigned int)0x9e3779b97f4a7c15l;
  return *seed;
}

unsigned int myrand_r3 (unsigned int *seed) {
  uint64_t wyhash64_x = *seed + 0x60bee2bee120fc15;
  __uint128_t tmp;
  tmp = (__uint128_t) wyhash64_x * 0xa3b195354a39b70d;
  uint64_t m1 = (tmp >> 64) ^ tmp;
  tmp = (__uint128_t)m1 * 0x1b03738712fad5c9;
  uint64_t m2 = (tmp >> 64) ^ tmp;
  *seed = (unsigned int) wyhash64_x;
  return (unsigned int) m2;
}

// You can also try using your own pseudorandom 
// generation tools
float myrand (unsigned int *seed) {
#if USE_MYRAND==1
    return myrand_r1 (seed) / (float)UINT_MAX;
#elif USE_MYRAND==2
    return myrand_r2 (seed) / (float)UINT_MAX;
#elif USE_MYRAND==3
    return myrand_r3 (seed) / (float)UINT_MAX;
#else
    return rand_r (seed) / (float)RAND_MAX;
#endif
}

double calculate_pi (long nPoints, int nThreads) {
  assert (nPoints > 0);
  assert (nThreads > 0);

  float x, y;
  unsigned int seed;
  long in;

   in = 0;
   seed = (unsigned)((unsigned long)(&seed)) ^ time(NULL);
   for (long i = 0;  i < nPoints;  i++) {
      x = myrand(&seed);
      y = myrand(&seed);
      if (x * x + y * y <= 1) {
        in++;
      }
    }    
  return (double)in * 4 / nPoints;
}


int main (int argc, char *argv[]) {
  
  // process command line flags  
  int ch;
  while ((ch = getopt(argc, argv, "sd")) != -1) {
    switch (ch) {
      case 's':
                silent = 1;
                break;
      case 'd':
                debug = 1;
                break;
      case '?':
      default:
                usage();
    }
  }
  argc -= optind;
  argv += optind;
  
  // get arguments
  if (argc != 2)
    usage ();
  long nPoints = atol (argv[0]);
  int nThreads = atoi (argv[1]);
  DEBUG(printf (" nPoints: %ld\n", nPoints));
  DEBUG(printf ("nThreads: %d\n", nThreads));
  
  // start timers for both read and CPU time
  struct timespec beginReal, beginCPU;
  clock_gettime (CLOCK_REALTIME, &beginReal);
  clock_gettime (CLOCK_PROCESS_CPUTIME_ID, &beginCPU);
  
  double myPI = calculate_pi (nPoints, nThreads);
  
  // stop timers
  struct timespec endReal, endCPU;
  clock_gettime (CLOCK_REALTIME, &endReal);
  clock_gettime (CLOCK_PROCESS_CPUTIME_ID, &endCPU);
  
  // calculate difference
  struct timespec timeReal, timeCPU;
  timespec_diff (&endReal, &beginReal, &timeReal);
  timespec_diff (&endCPU, &beginCPU, &timeCPU);

  // print times
  if (silent) {
    DEBUG(printf ("nPnt\tnThr\tReal\tCPU\tπ\n"));
    printf ("%ld\t%d\t%0.6lf\t%0.6lf\t%0.9lf\n", nPoints, nThreads, 
            timespec_to_milisecs(&timeReal), timespec_to_milisecs(&timeCPU),myPI);
  } else {
    printf ("\nπ: %0.9lf\n\n", myPI);
    printf ("Real Time:\t%0.6lf\n", timespec_to_milisecs(&timeReal));
    printf (" CPU Time:\t%0.6lf\n", timespec_to_milisecs(&timeReal));
  }
}