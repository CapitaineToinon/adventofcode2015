#include "common.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 1000
#define HEIGHT 1000
#define MATCHES_LEN 6
#define TURN_ON 1
#define TURN_OFF 2
#define TOGGLE 3

int mcmp(regmatch_t match, char *line, char *target) {
  return strncmp(line + match.rm_so, target, match.rm_eo - match.rm_so);
}

int get_mode(regmatch_t match, char *line) {
  if (mcmp(match, line, "turn on") == 0) {
    return TURN_ON;
  }

  if (mcmp(match, line, "turn off") == 0) {
    return TURN_OFF;
  }

  if (mcmp(match, line, "toggle") == 0) {
    return TOGGLE;
  }

  return -1;
}

int mtoi(regmatch_t match, char *line) { return atoi(line + match.rm_so); }

int main() {
  bool p1_lights[WIDTH * HEIGHT] = {false};
  int p2_lights[WIDTH * HEIGHT] = {0};

  FILE *file = fopen_orexit("./input/day06");
  char line[256];

  regex_t regex;
  regmatch_t matches[MATCHES_LEN];

  regcomp_orexit(&regex,
                 "(turn on|turn off|toggle) ([0-9]+),([0-9]+) through "
                 "([0-9]+),([0-9]+)",
                 REG_EXTENDED);

  while (fgets(line, sizeof(line), file)) {
    if (regexec(&regex, line, MATCHES_LEN, matches, 0) != 0) {
      printf("failed to execute regex\n");
      exit(EXIT_FAILURE);
    }

    int mode = -1;

    if ((mode = get_mode(matches[1], line)) == -1) {
      printf("failed to parse mode\n");
      exit(EXIT_FAILURE);
    }

    int from_x = mtoi(matches[2], line);
    int from_y = mtoi(matches[3], line);
    int to_x = mtoi(matches[4], line);
    int to_y = mtoi(matches[5], line);

    for (int x = from_x; x <= to_x; x++) {
      for (int y = from_y; y <= to_y; y++) {
        if (mode == TURN_ON) {
          p1_lights[y * WIDTH + x] = true;
          p2_lights[y * WIDTH + x] = p2_lights[y * WIDTH + x] + 1;
        }

        if (mode == TURN_OFF) {
          p1_lights[y * WIDTH + x] = false;
          p2_lights[y * WIDTH + x] = max2(p2_lights[y * WIDTH + x] - 1, 0);
        }

        if (mode == TOGGLE) {
          p1_lights[y * WIDTH + x] = !p1_lights[y * WIDTH + x];
          p2_lights[y * WIDTH + x] += 2;
        }
      }
    }
  };

  regfree(&regex);

  int p1_total = 0;
  int p2_total = 0;

  for (int i = 0; i < WIDTH * HEIGHT; i++) {
    if (p1_lights[i]) {
      p1_total++;
    }

    p2_total += p2_lights[i];
  }

  printf("%d\n", p1_total);
  printf("%d\n", p2_total);

  return 0;
}
