// Copyright (c) 2007 Intel Corp.

// Black-Scholes
// Analytical method for calculating European Options
//
//
// Reference Source: Options, Futures, and Other Derivatives, 3rd Edition,
// Prentice
// Hall, John C. Hull,

#include "options.h"

#define PAD 256
#define LINESIZE 64

fptype *prices;
int numOptions;

int *otype;
fptype *sptprice;
fptype *strike;
fptype *rate;
fptype *volatility;
fptype *otime;
int numError = 0;
int nThreads;

int bs_thread(void *tid_ptr);

int main(int argc, char **argv) {
  int loopnum;
  int rv;
  int i;
  fptype *buffer;
  int *buffer2;

  if (argc != 3) {
    printf("Usage:\n\t%s <nthreads> <noptions>\n", argv[0]);
    exit(1);
  }
  nThreads   = atoi(argv[1]);
  numOptions = atoi(argv[2]);

  if (nThreads > numOptions) {
    printf("WARNING: Not enough work, reducing number of threads to match "
           "number of options.\n");
    nThreads = numOptions;
  }

  // First option from simsmall
  OptionData option;

  option.s          = 42;
  option.strike     = 40;
  option.r          = 0.1000;
  option.divq       = 0.00;
  option.v          = 0.20;
  option.t          = 0.5;
  option.OptionType = 'C';
  option.divs       = 0.00;
  option.DGrefval   = 4.759423036851750055;

  pthread_mutexattr_init(&normalMutexAttr);
  numThreads = nThreads;
  {
    int i;
    for (i = 0; i < MAX_THREADS; i++) {
      threadsTableAllocated[i] = 0;
    }
  };
  printf("Num of Options: %d\n", numOptions);
  printf("Num of Runs: %d\n", NUM_RUNS);

  prices = (fptype *)malloc(numOptions * sizeof(fptype));

  buffer     = (fptype *)malloc(5 * numOptions * sizeof(fptype) + PAD);
  sptprice   = (fptype *)(((unsigned long long)buffer + PAD) & ~(LINESIZE - 1));
  strike     = sptprice + numOptions;
  rate       = strike + numOptions;
  volatility = rate + numOptions;
  otime      = volatility + numOptions;

  buffer2 = (int *)malloc(numOptions * sizeof(fptype) + PAD);
  otype   = (int *)(((unsigned long long)buffer2 + PAD) & ~(LINESIZE - 1));

  for (i = 0; i < numOptions; i++) {
    otype[i]      = (option.OptionType == 'P') ? 1 : 0;
    sptprice[i]   = option.s;
    strike[i]     = option.strike;
    rate[i]       = option.r;
    volatility[i] = option.v;
    otime[i]      = option.t;
  }

  printf("Size of data: %d\n", numOptions * (sizeof(OptionData) + sizeof(int)));

  int *tids;
  tids = (int *)malloc(nThreads * sizeof(int));

  for (i = 0; i < nThreads; i++) {
    tids[i] = i;

    {
      int i;
      for (i = 0; i < MAX_THREADS; i++) {
        if (threadsTableAllocated[i] == 0)
          break;
      }
      pthread_create(&threadsTable[i], NULL, (void *(*)(void *))bs_thread,
                     (void *)&tids[i]);
      threadsTableAllocated[i] = 1;
    };
  }

  {
    int i;
    void *ret;
    for (i = 0; i < MAX_THREADS; i++) {
      if (threadsTableAllocated[i] == 0)
        break;
      pthread_join(threadsTable[i], &ret);
    }
  };

  free(tids);
  free(prices);
  free(buffer);
  free(buffer2);

  return 0;
}

int bs_thread(void *tid_ptr) {
  int i, j;
  fptype price;
  fptype priceDelta;
  int tid   = *(int *)tid_ptr;
  int start = tid * (numOptions / nThreads);
  int end   = start + (numOptions / nThreads);

  for (j = 0; j < NUM_RUNS; j++) {
    for (i = start; i < end; i++) {
      /* Calling main function to calculate option value based on
       * Black & Scholes's equation.
       */
      price = BlkSchlsEqEuroNoDiv(sptprice[i], strike[i], rate[i],
                                  volatility[i], otime[i], otype[i], 0);
      prices[i] = price;
    }
  }

  return 0;
}
