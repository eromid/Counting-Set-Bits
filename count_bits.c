#include "stdio.h"
#include "inttypes.h"
#include "time.h"
#include "stdlib.h"

#define ITERATIONS 10000000u  // Number of iterations we will be testing over. 10000000 takes ~second/test on RaspberryPi 3.
#define N_CASES 6  // number of test cases

/*
  Input and expected output for testing correctness.
*/
static const uint32_t test_input[N_CASES] =    {0, 1, 2, 3, ~0, (~0)-1};
static const uint32_t test_expected[N_CASES] = {0, 1, 1, 2, 32,     31};

/*
  Convenient typedef for function pointer; takes one uint32, returns a uint32.
  We'll be passing our various bit counting functions to the testing function.
*/
typedef uint32_t(*bc_fn_ptr)(uint32_t);

/*
  Naive method to count set bits; right shift until nothing is left, checking
  the least significant bit each time.
*/
uint32_t count_bits_naive(uint32_t n)
{
  uint32_t count = 0;
  for (count = 0; n; n >>= 1)
  {
    count += n & 0x1;
  }
  return count;
}

/*
  The "fast" way, iterations = number of bits set.
  See https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan
*/
uint32_t count_bits_fast(uint32_t n)
{
  uint32_t count;
  for (count = 0; n; ++count)
  {
    n &= n - 1; // clears the least significant bit set
  }
  return count;
}

/*
  A handy way of initializing the lookup table.
  From https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetTable
*/
static const uint8_t BitsSetTable256[256] = 
{
#define B2(n)  n,     n+1,     n+1,     n+2
#define B4(n)  B2(n), B2(n+1), B2(n+1), B2(n+2)
#define B6(n)  B4(n), B4(n+1), B4(n+1), B4(n+2)
        B6(0), B6(1), B6(1),   B6(2)
};

/*
  Count set bits lookup table. From https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetTable
*/
uint32_t count_bits_lookup(uint32_t n)
{
  uint32_t count;
  uint8_t *p = (uint8_t *) &n;
  count = BitsSetTable256[p[0]] + 
          BitsSetTable256[p[1]] + 
          BitsSetTable256[p[2]] + 
          BitsSetTable256[p[3]];
  return count;
}

void test_bc_fn(bc_fn_ptr function, const char *fn_name)
{
  uint32_t count;
  int i;
  for (i = 0; i < N_CASES; ++i)
  {
    count = function(test_input[i]);
    if (count != test_expected[i])
    {
      fprintf(stderr,
              "%s failed test: input was %u, expected result was %u bits set, actual result was %u!\n",
              fn_name, test_input[i], test_expected[i], count);
    }
  }
}

/*
  A dummy function to use estimate the overhead of the tests.
*/
uint32_t dummy(uint32_t n)
{
  (void) n;
  return 0;
}

/*
  Function for testing a bit counting function.
  Takes a pointer to the function.
  Returns total elapsed time in MSECS.
*/
uint32_t time_bc_fn(bc_fn_ptr function)
{
  uint32_t count __attribute__((__unused__));  // We don't actually do anything with the count
  clock_t start = clock() / (CLOCKS_PER_SEC / 1000);
  for (uint32_t i = 0; i < ITERATIONS; ++i)
  {
    count = function(rand());
  }
  clock_t end = clock() / (CLOCKS_PER_SEC / 1000);
  return end - start;
}

int main(int argc, char const *argv[])
{
  (void) argc;
  (void) argv;
  test_bc_fn(count_bits_naive, "Naive method");
  test_bc_fn(count_bits_fast, "\"Fast\" method");
  test_bc_fn(count_bits_lookup, "Lookup table method");

  printf("Testing bit counting methods over %u iterations...\n", ITERATIONS);
  printf("(Test overhead is ~%u msecs)\n",  time_bc_fn(dummy));
  printf("Naive method:        %u msecs\n", time_bc_fn(count_bits_naive));
  printf("Fast method:         %u msecs\n", time_bc_fn(count_bits_fast));
  printf("Lookup table method: %u msecs\n", time_bc_fn(count_bits_lookup));
  return 0;
}
