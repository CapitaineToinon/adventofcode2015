#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int min(int a, int b, int c) {
  int min = a;

  if (b < min) {
    min = b;
  }

  if (c < min) {
    min = c;
  }

  return min;
}

int main() {
  FILE *file = fopen("./input/day02", "r");

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
    paper += min(area_a, area_b, area_c);
    ribbon += min(peri_a, peri_b, peri_c);
    ribbon += volume;
  }

  printf("%d\n", paper);
  printf("%d\n", ribbon);

  fclose(file);
  return 0;
}
