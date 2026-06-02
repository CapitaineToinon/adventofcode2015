#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  FILE *file = fopen_orexit("./input/day02");

  int paper = 0;
  int ribbon = 0;
  char line[256];

  while (fgets(line, sizeof(line), file)) {
    int l = atoi(strtok(line, "x"));
    int w = atoi(strtok(NULL, "x"));
    int h = atoi(strtok(NULL, "x"));

    int area_a = l * w, peri_a = 2 * l + 2 * w;
    int area_b = w * h, peri_b = 2 * w + 2 * h;
    int area_c = h * l, peri_c = 2 * h + 2 * l;
    int volume = l * w * h;

    paper += 2 * area_a + 2 * area_b + 2 * area_c;
    paper += min3(area_a, area_b, area_c);
    ribbon += min3(peri_a, peri_b, peri_c);
    ribbon += volume;
  }

  printf("%d\n", paper);
  printf("%d\n", ribbon);

  fclose(file);
  return 0;
}
