Complete the table below with your results, and then provide your interpretation at the end.

Note that:

- When calculating the parallel speed-up S, use the time output by the code, which corresponds
  to the parallel calculation and does not include initialising the matrix/vectors or
  performing the serial check.

- Take as the serial execution time the time output by the code when run with a single process
  (hence the speed-up for 1 process should be 1.0).

All runs were performed with N = 12000

No. Process:                        Mean time (average of 3 runs)           Parallel speed-up, S:
===========                         ============================:           ====================
1                                   0.984031                                1.000000
2                                   0.569691                                1.727307
3                                   0.913598                                1.077094
4                                   0.535975                                1.835964

Architecture that the timing runs were performed on:

Ran on my personal laptop.
OS: Linux
CPU: Intel i7-7500u @ 2.7GHz; 2 Cores with 4 hyperthreaded logical threads

A brief interpretation of these results (2-3 sentences should be enough):

When the number of processes was a multiple of the number of CPU cores, the parallel speedup was almost 2.
This makes sense because at any point in the program execution, all the CPU cores could be saturated and
executing one of the MPI processes. Since my CPU has two cores, this meant that the two halves of the matrix-
vector multiplication could be run in parallel, and thus this results in a near 2x speedup when compared to
the serial implementation.

In contrast, when the number of processes was three, there was little parallel speed up. Two of the three
processes would have executed in parallel on the CPU, one on each core. They would finish their execution at a similar
time, and when one was done, the final process would be allocated to a core. This results in one of the processes
not executing in parallel with any other. Overall, we see roughly a run time that is double the time required to
run a third of the computation in serial. Thus, we get a speedup that is slightly better than the serial implementation,
but not by much.