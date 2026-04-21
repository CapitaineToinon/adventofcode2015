#include <stdio.h>

int main() {
  FILE *file = fopen("./input/day01", "r");

  char ch;
  int step = -1;
  int floor = 0;
  int i = 1;

  while ((ch = fgetc(file)) != EOF) {
    if (ch == '(') {
      floor++;
    }

    if (ch == ')') {
      floor--;
    }

    if (step == -1 && floor < 0) {
      step = i;
    }

    i++;
  }

  printf("%d\n", floor);
  printf("%d\n", step);
  fclose(file);
  return 0;
}
