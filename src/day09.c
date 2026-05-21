#include <limits.h>
#include <regex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_LEN 256
#define LINE_ITEM_COUNT 4
#define MAX_CITIES 10
#define MAX_CITY_NAME 50

typedef struct {
  int count;
  char names[MAX_CITIES][MAX_CITY_NAME];
  int edges[(MAX_CITIES * (MAX_CITIES - 1)) / 2];
} ctx_t;

regex_t regex;
ctx_t context;

int min(int a, int b) { return a < b ? a : b; }
int max(int a, int b) { return a > b ? a : b; }

int add_city(ctx_t *ctx, char *city, int city_len) {
  for (int i = 0; i < ctx->count; i++) {
    if (strncmp(ctx->names[i], city, city_len) == 0) {
      return i;
    }
  }

  if (ctx->count == MAX_CITIES) {
    printf("Reached maximum amount of cities allowed of %d\n", MAX_CITIES);
    exit(-1);
  }

  strncpy(ctx->names[ctx->count++], city, city_len);
  return ctx->count - 1;
}

int add_city_from_match(ctx_t *ctx, char *line, regmatch_t m) {
  return add_city(ctx, line + m.rm_so, m.rm_eo - m.rm_so);
}

void add_edge(ctx_t *ctx, int from, int to, int distance) {
  int index = (1 << from) | (1 << to);
  ctx->edges[index] = distance;
}

int solve(ctx_t *ctx, uint32_t from, int visited, int total,
          int (*cmp)(int, int), int default_result) {
  if (visited == (1 << ctx->count) - 1) {
    return total;
  }

  visited |= (1 << from);

  int result = default_result;

  for (int to = 0; to < ctx->count; to++) {
    if (((visited >> to) & 1) == 1) {
      continue;
    }

    int distance = ctx->edges[(1 << from) | (1 << to)];
    int local = solve(ctx, to, visited | (1 << to), total + distance, cmp,
                      default_result);

    result = cmp(result, local);
  }

  return result;
}

int solve_all(ctx_t *ctx, int (*cmp)(int, int), int default_result) {
  int result = default_result;

  for (int city = 0; city < ctx->count; city++) {
    int distance = solve(&context, city, 0, 0, cmp, default_result);
    result = cmp(result, distance);
  }

  return result;
}

int main() {
  FILE *file = fopen("./input/day09", "r");
  char line[256];

  if (regcomp(&regex, "([a-zA-Z]+) to ([a-zA-Z]+) = ([0-9]+)",
              REG_NEWLINE | REG_EXTENDED) != 0) {
    printf("failed to compile regex\n");
    exit(-1);
  }

  regmatch_t matches[LINE_ITEM_COUNT];

  while (fgets(line, LINE_LEN, file)) {
    if (regexec(&regex, line, LINE_ITEM_COUNT, matches, 0) != 0) {
      printf("line is invalid: %s\n", line);
      exit(-1);
    }

    int from = add_city_from_match(&context, line, matches[1]);
    int to = add_city_from_match(&context, line, matches[2]);
    int distance = atoi(line + matches[3].rm_so);
    add_edge(&context, from, to, distance);
  }

  fclose(file);

  printf("%d\n", solve_all(&context, &min, INT_MAX));
  printf("%d\n", solve_all(&context, &max, 0));

  return 0;
}
