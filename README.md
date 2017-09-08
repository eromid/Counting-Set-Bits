# Counting Set Bits #

This little project came about after a friend was asked how to count set bits in a technical interview. The preferred solution of the interviewers seemed to be [Brian Kernighan's algorithm](https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan) (Sean Anderson points out its discovery predates Kernighan's publication of it). I wondered if a lookup table might be faster since it avoids any branching, so I wrote some C...

# Algorithms #

## Naive ##
The most naive method is performing a right shift by one bit in a loop and incrementing a counter when there is a set bit in the least significant position. This will result in a fixed number of iterations, equal to the number of bits in the input, although we can reduce this by instead stopping when there are no set bits left, i.e. when the number equals zero.

## Kernighan's ##

Kernighan's algorithm again uses a loop, however this time we don't go bit by bit, but clear the least significant bit each iteration until there are no set bits left. We count the number of iterations as we go and when we are done, this is equal to the number of set bits we have cleared.

```
uint32_t count_bits_fast(uint32_t n)
{
  uint32_t count;
  for (count = 0; n; ++count)
  {
    n &= n - 1; // clears the least significant bit set
  }
  return count;
}
```
Since the number of iterations is equal to the number of bits set, in the worst case we do as many iterations as there are bits (i.e. if all bits are set) and in the best case we do zero iterations, just the test to see if `n > 0`.

## Lookup Table ##

Even in the best case, Kernighan's algorithm still causes branching, since execution is conditional on `n`. With modern CPUs using pipelining, [branching typically has a negative impact on performance](https://en.wikipedia.org/wiki/Branch_(computer_science)#Performance_problems_with_branch_instructions).

With a lookup table, we store a mapping between the bit pattern (i.e. the number) and the number of set bits this equates to, and just look up the relevant value from the table. The only thing conditional on the input is which value to fetch from the table, thus no branching is needed.

The length of the table depends on the input and is equal to 2^n_bits. For large numbers of bits this poses a problem: we don't want to store a table of 2^32 or 2^64 integers in main memory! To counter this, we simply split the input into manageable chunks and use a table sufficient to manage these. There is a space vs. time trade-off here, we want the table to be nice and short to maximize cache hits, but splitting the input involves some (minor) overhead. In my implementation the table has 256 entries since we handle 8 bits chunks. I haven't tested other sizes, though I wouldn't be surprised if it was optimal.

# Results #
Running on a raspberry pi 3, with random 32 bit unsigned integers:
```
Testing bit counting methods over 10000000 iterations...
(Test overhead is ~781 msecs)
Naive method:        1414 msecs
Fast method:         1172 msecs
Lookup table method: 860 msecs
```
The lookup table looks like the fastest method. One caveat of this result is that the input is random, and since Kernighan's algorithm's runtime depends on the number of bits set, the results would differ if the was a non uniform distribution of the number of set bits in the inputs. If your program is counting bits in numbers which tend to be small, then it might perform better.

My own intuition tells me in general you are more likely to have integers in your program which have less than half of the bits set (since smaller numbers are more prevalent?). On the other hand if you are counting bits it suggests you are storing an [array of bits](https://en.wikipedia.org/wiki/Bit_array) rather than a number and the previous assumption may not be valid in that case. Either way, if I ever need to do this for real, more testing would be required.

# Conclusion #

We've seen that a lookup table can potentially count set bits faster than even a "clever" iterative solution. There is a space vs. time trade-off to be made, which is generally the case when tackling these kinds of problems with lookup tables.

If you found this interesting, check out [memoization](https://en.wikipedia.org/wiki/Memoization), a similar concept but the table is filled in at run time so that if we call the function again with the same input, we can just return the cached result.
