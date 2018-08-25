#include <stdio.h>
#include <ctype.h>
#include "picosat.h"

#define VAR(r, c, d) (1 + 81*(r) + 9*(c) + (d))
#define BOX(b, z, d) VAR(3*((b)/3) + (z)/3, 3*((b)%3) + (z)%3, d)
#define ADD_POS(p, r, c, d) picosat_add(p, VAR(r, c, d))
#define ADD_NEG(p, r, c, d) picosat_add(p, -VAR(r, c, d))
#define ADD_POS_BOX(p, b, z, d) picosat_add(p, BOX(b, z, d))
#define ADD_NEG_BOX(p, b, z, d) picosat_add(p, -BOX(b, z, d))
#define END_CLAUSE(p) picosat_add(p, 0)


void
gen_cell_filled(PicoSAT *p) {
  for (int r = 0; r < 9; r++) {
    for (int c = 0; c < 9; c++) {
      // Add a constraint for each row+col
      for (int d = 0; d < 9; d++) {
        ADD_POS(p, r, c, d);
      }
      END_CLAUSE(p);
    }
  }
}

void
gen_digit_used_row(PicoSAT *p) {
  for (int c = 0; c < 9; c++) {
    for (int d = 0; d < 9; d++) {
      // Add a constraint for each col+digit
      for (int r = 0; r < 9; r++) {
        ADD_POS(p, r, c, d);
      }
      END_CLAUSE(p);
    }
  }
}

void gen_digit_used_col(PicoSAT *p) {
  for (int r = 0; r < 9; r++) {
    for (int d = 0; d < 9; d++) {
      // Add a constraint for each row+digit
      for (int c = 0; c < 9; c++) {
        ADD_POS(p, r, c, d);
      }
      END_CLAUSE(p);
    }
  }
}

void gen_digit_used_box(PicoSAT *p) {
  for (int b = 0; b < 9; b++) {
    for (int d = 0; d < 9; d++) {
      // Add a constraint for each box+digit
      for (int z = 0; z < 9; z++) {
        ADD_POS_BOX(p, b, z, d);
      }
      END_CLAUSE(p);
    }
  }
}

void all_diff_cell(PicoSAT *p) {
  for (int r = 0; r < 9; r++) {
    for (int c = 0; c < 9; c++) {
      for (int d1 = 0; d1 < 8; d1++) {
        for (int d2 = d1 + 1; d2 < 9; d2++) {
          ADD_NEG(p, r, c, d1);
          ADD_NEG(p, r, c, d2);
          END_CLAUSE(p);
        }
      }
    }
  }
}

void all_diff_row(PicoSAT *p) {
  for (int c = 0; c < 9; c++) {
    for (int d = 0; d < 9; d++) {
      for (int r1 = 0; r1 < 8; r1++) {
        for (int r2 = r1 + 1; r2 < 9; r2++) {
          ADD_NEG(p, r1, c, d);
          ADD_NEG(p, r2, c, d);
          END_CLAUSE(p);
        }
      }
    }
  }
}

void all_diff_box(PicoSAT *p) {
  for (int b = 0; b < 9; b++) {
    for (int d = 0; d < 9; d++) {
      for (int z1 = 0; z1 < 8; z1++) {
        for (int z2 = z1 + 1; z2 < 9; z2++) {
          ADD_NEG_BOX(p, b, z1, d);
          ADD_NEG_BOX(p, b, z2, d);
          END_CLAUSE(p);
        }
      }
    }
  }
}

void all_diff_col(PicoSAT *p) {
  for (int r = 0; r < 9; r++) {
    for (int d = 0; d < 9; d++) {
      for (int c1 = 0; c1 < 8; c1++) {
        for (int c2 = c1 + 1; c2 < 9; c2++) {
          ADD_NEG(p, r, c1, d);
          ADD_NEG(p, r, c2, d);
          END_CLAUSE(p);
        }
      }
    }
  }
}

void print_result(PicoSAT *p, char *line) {
  int buf[81];
  int n = picosat_variables(p);
  for (int i = 0, k = 0; i < n; i++) {
    if (picosat_deref(p, i + 1) == 1) {
      buf[k++] = i % 9 + 1;
    }
  }

  for (int r = 0; r < 9; r++) {
    for (int c = 0; c < 9; c++) {
      putchar(line[r*9+c]);
    }
    printf("   ");
    for (int c = 0; c < 9; c++) {
      putchar(buf[r*9+c] + '0');
    }
    putchar('\n');
  }
  printf("\n\n");
}

int
main() {
  PicoSAT *p = picosat_init();

  // These constraints are added with picosat_add(),
  // and they are permanent. They define the rules of
  // Sudoku.
  gen_digit_used_row(p);
  gen_digit_used_col(p);
  gen_digit_used_box(p);
  all_diff_cell(p);

  // These constraints are not actually required, but
  // make the solver go a lot faster.  Try commenting
  // them out and you'll see what I mean!
  all_diff_row(p);
  all_diff_col(p);
  all_diff_box(p);

  // Read problems line by line, in the format
  // 5.7.....3...2...1...........1..3....2......4.....5.7.....1.4......6..4..8.....5..
  char buf[100];
  char *line;
  while ((line = fgets(buf, 99, stdin)) != 0) {
    // These constraints are added with picosat_assume(),
    // and picosat clears them after each run. They assert
    // the positive (row, col, digit) literal for each given
    // square in the problem.
    for (int i = 0; i < 81; i++) {
      if (isdigit(line[i])) {
        picosat_assume(p, i * 9 + (line[i] - '0'));
      }
    }

    int result = picosat_sat(p, -1);
    switch (result) {
      case PICOSAT_UNSATISFIABLE:
        printf("Unsat\n");
        break;

      case PICOSAT_SATISFIABLE:
        print_result(p, line);
        break;

      case PICOSAT_UNKNOWN:
        // This would happen if we had set constraints
        // on the solver, like "only try N decisions".
        printf("Unknown\n");
        break;
    }
  }
}
