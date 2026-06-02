#include "common.h"
#include <limits.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>

#define LINE_LEN 256
#define GROUPS_LEN 7
#define INGREDIENTS_LEN 10
#define PROPERTIES_LEN 5
#define CALORIES_INDEX 4
#define INGREDIENTS_TOTAL 100

int score(int **ingredients, int *values, int len, int calories_target) {
  if (calories_target != -1) {
    // limit recepies that only achieve a specific
    // calories target, skip otherwise
    int calories = 0;

    for (int i = 0; i < len; i++) {
      calories += (values[i] * ingredients[i][CALORIES_INDEX]);
    }

    if (calories != 500) {
      return 0;
    }
  }

  int total = 1;

  for (int p = 0; p < PROPERTIES_LEN; p++) {
    if (p == CALORIES_INDEX) {
      continue;
    }

    int ptotal = 0;

    for (int i = 0; i < len; i++) {
      ptotal += (values[i] * ingredients[i][p]);
    }

    if (ptotal <= 0) {
      return 0;
    }

    total *= ptotal;
  }

  return total;
}

int solve(int **ingredients, int *values, int len, int i, int target, int best,
          int calories_target) {
  if (i == len - 1) {
    values[i] = target;
    return max2(score(ingredients, values, len, calories_target), best);
  }

  int local = best;

  for (int value = 0; value < target - i; value++) {
    values[i] = value;
    local = solve(ingredients, values, len, i + 1, target - value, local,
                  calories_target);
  }

  return local;
}

int main() {
  regex_t regex;
  regmatch_t matches[GROUPS_LEN];

  regcomp_orexit(
      &regex,
      "[a-zA-Z]+: capacity (-?[0-9]+), durability (-?[0-9]+), flavor "
      "(-?[0-9]+), texture (-?[0-9]+), "
      "calories (-?[0-9]+)",
      REG_EXTENDED);

  FILE *file = fopen_orexit("./input/day15");
  char line[LINE_LEN];
  int len = 0;
  int **ingredients = malloc(sizeof(int *) * INGREDIENTS_LEN);

  while (fgets(line, LINE_LEN, file)) {
    if (regexec(&regex, line, GROUPS_LEN, matches, 0) != 0) {
      printf("invalid line\n");
      exit(EXIT_FAILURE);
    }

    if (len == INGREDIENTS_LEN) {
      printf("max ingredients count reached\n");
      exit(EXIT_FAILURE);
    }

    ingredients[len] = malloc(sizeof(int) * PROPERTIES_LEN);

    for (int p = 0; p < PROPERTIES_LEN; p++) {
      ingredients[len][p] = atoi(line + matches[1 + p].rm_so);
    }

    len++;
  }

  fclose(file);
  regfree(&regex);

  int *values = malloc(sizeof(int) * len);
  int calories_target = -1, best = 0, start = 0;

  int part_1 = solve(ingredients, values, len, start, INGREDIENTS_TOTAL, best,
                     calories_target);

  calories_target = 500;
  int part_2 = solve(ingredients, values, len, start, INGREDIENTS_TOTAL, best,
                     calories_target);

  printf("%d\n", part_1);
  printf("%d\n", part_2);

  free(values);

  for (int i = 0; i < len; i++) {
    free(ingredients[i]);
  }

  free(ingredients);

  return 0;
}
