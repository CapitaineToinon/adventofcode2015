#include <regex.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 500
#define N_COMPOUNDS 10
#define N_GROUPS 2
#define UNSET -1

typedef struct solution {
  int best;
  int sue;
} solution_t;

typedef enum compound {
  CHILDREN = 0,
  CATS = 1,
  SAYMOYEDS = 2,
  POMERANIANS = 3,
  AKITAS = 4,
  VIZSLAS = 5,
  GOLDFISH = 6,
  TREES = 7,
  CARS = 8,
  PERFUMES = 9,
} compound_t;

const char *COMPOUND_NAMES[N_COMPOUNDS] = {
    "children", "cats",     "samoyeds", "pomeranians", "akitas",
    "vizslas",  "goldfish", "trees",    "cars",        "perfumes",
};

const int input[N_COMPOUNDS] = {3, 7, 2, 3, 0, 0, 5, 3, 2, 1};

int p1_similarity(int sue[N_COMPOUNDS], const int values[N_COMPOUNDS]) {
  int score = 0;

  for (int i = 0; i < N_COMPOUNDS; i++) {
    if (sue[i] == UNSET) {
      continue;
    }

    if (values[i] == sue[i]) {
      score++;
    }
  }

  return score;
}

int p2_similarity(int sue[N_COMPOUNDS], const int values[N_COMPOUNDS]) {
  int score = 0;

  for (int i = 0; i < N_COMPOUNDS; i++) {
    if (sue[i] == UNSET) {
      continue;
    }

    switch (i) {
    case CATS:
    case TREES:
      if (sue[i] > values[i]) {
        score++;
      }
      break;
    case POMERANIANS:
    case GOLDFISH:
      if (sue[i] < values[i]) {
        score++;
      }
      break;
    default:
      if (sue[i] == values[i]) {
        score++;
      }
      break;
    }
  }

  return score;
}

void init_solution(solution_t *solution) {
  solution->best = 0;
  solution->sue = UNSET;
}

int main() {
  regex_t regexes[N_COMPOUNDS];

  for (int i = 0; i < N_COMPOUNDS; i++) {
    char pattern[MAX_LINE] = {};

    if (snprintf(pattern, MAX_LINE, "%s: ([0-9]+)", COMPOUND_NAMES[i]) < 0) {
      printf("failed to create pattern\n");
      exit(-1);
    }

    if (regcomp(&regexes[i], pattern, REG_EXTENDED) != 0) {
      printf("failed to compile regex\n");
      exit(-1);
    }
  }

  FILE *file = fopen("./input/day16", "r");
  char line[MAX_LINE];

  solution_t p1;
  solution_t p2;
  init_solution(&p1);
  init_solution(&p2);

  int sue_i = 1;
  regmatch_t matches[N_GROUPS];

  while (fgets(line, MAX_LINE, file)) {
    int sue[N_COMPOUNDS];

    for (int i = 0; i < N_COMPOUNDS; i++) {
      if (regexec(&regexes[i], line, N_GROUPS, matches, 0) == 0) {
        sue[i] = atoi(line + matches[1].rm_so);
      } else {
        sue[i] = UNSET;
      }
    }

    int p1_score = p1_similarity(sue, input);

    if (p1_score > p1.best) {
      p1.best = p1_score;
      p1.sue = sue_i;
    }

    int p2_score = p2_similarity(sue, input);

    if (p2_score > p2.best) {
      p2.best = p2_score;
      p2.sue = sue_i;
    }

    sue_i++;
  }

  fclose(file);

  printf("%d\n", p1.sue);
  printf("%d\n", p2.sue);

  return 0;
}
