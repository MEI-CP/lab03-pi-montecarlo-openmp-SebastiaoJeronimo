#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <limits.h>
#include <stdint.h>
#include <assert.h>
#include <omp.h>

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
  printf ("Usage: ./example nIters nThreads\n\
             nIters: The total number of iterations.\n\
            nThreads: The number of working threads.");
  exit (1);
}

long calculate (long nIters, int nThreads) {
  assert (nThreads > 0);
  assert (nIters > nThreads);

  long in = 0;
  omp_set_dynamic(0);     // Explicitly disable dynamic teams
  omp_set_num_threads(nThreads); // Use nThreads threads for all consecutive parallel regions
  #pragma omp parallel for reduction(+:in)
  for (long i = 0;  i < nIters;  i++) {
    in++;
  }   
  return in;
}


int main (int argc, char *argv[]) {
  
  // process command line flags  
  int ch;
  while ((ch = getopt(argc, argv, "sd")) != -1) {
    switch (ch) {
      case '?':
      default:
        usage();
    }
  }
  argc -= optind;
  argv += optind;
  
  // get arguments
  if (argc != 2)
    usage();
  long nIters = atol (argv[0]);
  int nThreads = atoi (argv[1]);
  printf (" nIters: %ld\n", nIters);
  printf ("nThreads: %d\n", nThreads);
  
  // start timers for both read and CPU time
  struct timespec beginReal, beginCPU;
  clock_gettime (CLOCK_REALTIME, &beginReal);
  clock_gettime (CLOCK_PROCESS_CPUTIME_ID, &beginCPU);
  
  double val = calculate(nIters, nThreads);
  
  // stop timers
  struct timespec endReal, endCPU;
  clock_gettime (CLOCK_REALTIME, &endReal);
  clock_gettime (CLOCK_PROCESS_CPUTIME_ID, &endCPU);
  
  // calculate difference
  struct timespec timeReal, timeCPU;
  timespec_diff (&endReal, &beginReal, &timeReal);
  timespec_diff (&endCPU, &beginCPU, &timeCPU);

  // print times
  printf ("\nOutput value: %0.9lf\n\n", val);
  printf ("Real Time:\t%0.6lf\n", timespec_to_milisecs(&timeReal));
  printf (" CPU Time:\t%0.6lf\n", timespec_to_milisecs(&timeReal));
}