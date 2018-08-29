SAT-based sudoku solver

This is a very simple solver that uses the [Picosat][1] library
to do the hard work.

```
make
./doku < test/sudoku17.49151.txt
```

One notable thing about this program is that it runs much faster if
you add more constraints.  There are three lines in `main` as follows:

```
  all_diff_row(p);
  all_diff_col(p);
  all_diff_box(p);
```

If you comment out those lines, the program takes about 50 seconds to
solve the first 20 problems in `test/sudoku17.49151.txt`.  With the
lines added, it takes under 0.01 seconds, and can run the whole set
of 49,151 problems in about 18s on my laptop.


[1]: http://fmv.jku.at/picosat/
