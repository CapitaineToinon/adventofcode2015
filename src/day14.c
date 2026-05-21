#include <limits.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>

#define GROUP_LEN 5
#define LINE_LEN 256
#define MAX_REINDEERS 10
#define DURATION 2503

typedef struct {
  int speed_distance;
  int speed_seconds;
  int resting_seconds;
} reindeer_t;

int min(int a, int b) { return a < b ? a : b; }

int max(int *distances, int count) {
  int result = 0;

  for (int i = 0; i < count; i++) {
    if (distances[i] > result) {
      result = distances[i];
    }
  }

  return result;
}

int run_for(reindeer_t *r, int duration) {
  int loop_s = r->speed_seconds + r->resting_seconds;
  int full_loop_count = duration / loop_s;
  int loop_remainder = duration % loop_s;
  int speed_s_remainder = min(r->speed_seconds, loop_remainder);

  return (full_loop_count * r->speed_distance * r->speed_seconds) +
         (speed_s_remainder * r->speed_distance);
}

int main() {
  FILE *file = fopen("./input/day14", "r");
  char line[LINE_LEN];

  regex_t regex;
  regmatch_t matches[GROUP_LEN];

  if (regcomp(&regex,
              "([a-zA-Z]+) can fly ([0-9]+) km/s for ([0-9]+) seconds, but "
              "then must rest for "
              "([0-9]+) seconds.",
              REG_EXTENDED) != 0) {
    printf("failed to compile regex\n");
    exit(-1);
  }

  int count = 0;
  reindeer_t reindeers[MAX_REINDEERS];
  int distances[MAX_REINDEERS] = {0};
  int points[MAX_REINDEERS] = {-1};

  while (fgets(line, LINE_LEN, file)) {
    if (regexec(&regex, line, GROUP_LEN, matches, 0) != 0) {
      printf("invalid line\n");
      exit(-1);
    }

    if (count == MAX_REINDEERS) {
      printf("max reindeer count reached\n");
      exit(-1);
    }

    reindeers[count].speed_distance = atoi(line + matches[2].rm_so);
    reindeers[count].speed_seconds = atoi(line + matches[3].rm_so);
    reindeers[count].resting_seconds = atoi(line + matches[4].rm_so);
    count++;
  }

  for (int second = 1; second <= DURATION; second++) {
    int best = max(distances, count);

    for (int deer = 0; deer < count; deer++) {
      if (distances[deer] == best) {
        points[deer]++;
      }

      distances[deer] = run_for(&reindeers[deer], second);
    }
  }

  fclose(file);

  int part_1 = max(distances, count);
  int part_2 = max(points, count);

  printf("%d\n", part_1);
  printf("%d\n", part_2);

  return 0;
}
