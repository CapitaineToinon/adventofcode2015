#include "common.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_CONTAINERS 20
#define MAX_LINE 10
#define EGGNOG_LITERS 150

typedef struct context {
  int solutions;
  int min;
  int min_count;
} context_t;

void solve(int *containers, int size, int i, int used_container_count,
           int remainder, context_t *ctx) {
  if (i == size) {
    if (remainder == 0) {
      ctx->solutions++;

      if (used_container_count < ctx->min) {
        ctx->min = used_container_count;
        ctx->min_count = 0;
      }

      if (used_container_count == ctx->min) {
        ctx->min_count += 1;
      }
    }

    return;
  }

  // try skipping the current container
  solve(containers, size, i + 1, used_container_count, remainder, ctx);

  // if possible, try using the current container
  if (containers[i] <= remainder) {
    solve(containers, size, i + 1, used_container_count + 1,
          remainder - containers[i], ctx);
  }
}

int main() {
  FILE *file = fopen_orexit("./input/day17");

  char line[MAX_LINE];
  int size = 0;
  int containers[MAX_CONTAINERS];

  while (fgets(line, MAX_LINE, file)) {
    if (size > MAX_CONTAINERS) {
      printf("max container count reached\n");
      exit(EXIT_FAILURE);
    }

    containers[size++] = atoi(line);
  }

  context_t ctx;
  ctx.solutions = 0;
  ctx.min = INT_MAX;
  ctx.min_count = 0;
  int current_container = 0;
  int used_container_count = 0;

  solve(containers, size, current_container, used_container_count,
        EGGNOG_LITERS, &ctx);

  fclose(file);
  printf("%d\n", ctx.solutions);
  printf("%d\n", ctx.min_count);

  return 0;
}
