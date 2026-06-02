#include "common.h"
#include <stdio.h>
#include <string.h>

#define SIZE 100
#define LINE_SIZE (SIZE + 2) // SIZE + \n + \0
#define ON '#'
#define OFF '.'

void process(char grid[SIZE][SIZE], char next[SIZE][SIZE]) {
  for (int y = 0; y < SIZE; y++) {
    for (int x = 0; x < SIZE; x++) {
      int count = 0;

      for (int y_hat = y - 1; y_hat <= y + 1; y_hat++) {
        for (int x_hat = x - 1; x_hat <= x + 1; x_hat++) {
          if (y_hat < 0 || y_hat == SIZE || x_hat < 0 || x_hat == SIZE) {
            continue;
          }

          if (grid[y_hat][x_hat] == ON) {
            count++;
          }
        }
      }

      if (grid[y][x] == ON) {
        if (count == 3 || count == 4) {
          next[y][x] = ON;
        } else {
          next[y][x] = OFF;
        }
      }

      if (grid[y][x] == OFF) {
        if (count == 3) {
          next[y][x] = ON;
        } else {
          next[y][x] = OFF;
        }
      }
    }
  }
}

int count_on(char grid[SIZE][SIZE]) {
  int count = 0;

  for (int y = 0; y < SIZE; y++) {
    for (int x = 0; x < SIZE; x++) {
      if (grid[y][x] == ON) {
        count++;
      }
    }
  }

  return count;
}

int solve(char grid[SIZE][SIZE], int steps, bool force_corners) {
  if (force_corners) {
    grid[0][0] = ON;
    grid[0][SIZE - 1] = ON;
    grid[SIZE - 1][0] = ON;
    grid[SIZE - 1][SIZE - 1] = ON;
  }

  if (steps == 0) {
    return count_on(grid);
  }

  char next[SIZE][SIZE];
  process(grid, next);
  return solve(next, steps - 1, force_corners);
}

int main() {
  char grid[SIZE][SIZE];
  FILE *file = fopen_orexit("./input/day18");

  for (int y = 0; y < SIZE; y++) {
    char line[LINE_SIZE];
    fgets(line, LINE_SIZE, file);
    memcpy(grid[y], line, SIZE);
  }

  fclose(file);

  int part_1 = solve(grid, 100, false);
  int part_2 = solve(grid, 100, true);

  printf("%d\n", part_1);
  printf("%d\n", part_2);

  return 0;
}
