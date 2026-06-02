#include "common.h"
#include <limits.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_LEN 256
#define MAX_PEOPLE 10
#define GROUP_LEN 5
#define MAX_NAME_LEN 20

typedef struct {
  int count;
  char names[MAX_PEOPLE][MAX_NAME_LEN];
  int constraints[MAX_PEOPLE][MAX_PEOPLE];
} context_t;

context_t *create_context() {
  context_t *c = calloc(1, sizeof(context_t));
  return c;
}

int add_person(context_t *c, char *name, int len) {
  for (int i = 0; i < c->count; i++) {
    if (strncmp(c->names[i], name, len) == 0) {
      return i;
    }
  }

  strncpy(c->names[c->count], name, len);
  return c->count++;
}

int add_person_from_match(context_t *c, char *line, regmatch_t m) {
  char *name = line + m.rm_so;
  int len = m.rm_eo - m.rm_so;
  return add_person(c, name, len);
}

void add_constraint(context_t *c, int a, int b, int quantity) {
  c->constraints[a][b] = quantity;
}

void swap(int *seats, int a, int b) {
  int tmp = seats[a];
  seats[a] = seats[b];
  seats[b] = tmp;
}

int score(context_t *c, int *seats) {
  int total = 0;

  for (int cur_i = 0; cur_i < c->count; cur_i++) {
    int left_i = ((cur_i - 1) % c->count + c->count) % c->count;
    int right_i = (cur_i + 1) % c->count;

    int cur = seats[cur_i];
    int left = seats[left_i];
    int right = seats[right_i];

    total += c->constraints[cur][left];
    total += c->constraints[cur][right];
  }

  return total;
}

int permutations(context_t *c, int *seats, int k, int solution) {
  if (k == 1) {
    return score(c, seats);
  }

  int next = solution;

  for (int i = 0; i < k; i++) {
    next = max2(permutations(c, seats, k - 1, next), next);

    if (i < k - 1) {
      if (k % 2 == 0) {
        swap(seats, i, k - 1);
      } else {
        swap(seats, 0, k - 1);
      }
    }
  }

  return next;
}

int solve(context_t *c) {
  int *seats = malloc(sizeof(int) * c->count);

  for (int i = 0; i < c->count; i++) {
    seats[i] = i;
  }

  int answer = permutations(c, seats, c->count, INT_MIN);

  free(seats);

  return answer;
}

int main() {
  FILE *file = fopen_orexit("./input/day13");
  char line[LINE_LEN];

  context_t *ctx = create_context();
  regex_t regex;

  regcomp_orexit(&regex,
                 "([a-zA-Z]+) would (gain|lose) ([0-9]+) happiness units by "
                 "sitting next to ([a-zA-Z]+)\\.",
                 REG_EXTENDED);

  regmatch_t matches[GROUP_LEN];

  while (fgets(line, LINE_LEN, file)) {
    if (regexec(&regex, line, GROUP_LEN, matches, 0) != 0) {
      printf("invalid line\n");
      exit(EXIT_FAILURE);
    }

    int a = add_person_from_match(ctx, line, matches[1]);
    int b = add_person_from_match(ctx, line, matches[4]);
    int quantity = atoi(line + matches[3].rm_so);

    if (strncmp(line + matches[2].rm_so, "lose", 4) == 0) {
      quantity = -quantity;
    }

    add_constraint(ctx, a, b, quantity);
  }

  fclose(file);
  regfree(&regex);

  printf("%d\n", solve(ctx));
  add_person(ctx, "Me", 2);
  printf("%d\n", solve(ctx));

  free(ctx);

  return 0;
}
