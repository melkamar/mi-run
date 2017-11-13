
# MI-RUN, GC seminar

Clone the repository using

```git clone https://github.com/japod/mi-run-gc.git```

Then compile and run using gcc:

```cd mi-run-gc; gcc mas.c && ./a.out```

If you see something like follows, you are ready.
~~~~
  alloc:  2041
  free:   7
  MAX:    2048
  [0]: oook (3)->oook->oook->ook!
  [1]: ook! (0)
  [2]: oook (0)
  [3]: oook (0)
  [4]: ook (4)->oook->ook->oook->oook
  [5]: ook (4)->oook->oook->oook->ook!
  [6]: ook! (0)
  [7]: oook (1)->oook
  
  alloc:  2047
  free:   1
  MAX:    2048
  [0]: oook (3)->oook->oook->ook!
  [1]: ook! (1)->oook
  [2]: oook (0)
  [3]: oook (0)
  [4]: ook! (0)
  [5]: ook (4)->oook->oook->oook->ook!
  [6]: ook (5)->ook->oook->oook->oook->ook!
  [7]: ook (1)->oook
  Out of memory, bye!
~~~~
