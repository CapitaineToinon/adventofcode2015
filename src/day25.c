#include <stdint.h>
#include <stdio.h>

int triangle(int n) { return (n * (n + 1)) / 2; }

int get_position(int x, int y) { return triangle((x - 1) + y) - (y - 1); }

int main() {
  int position = get_position(3075, 2981);
  uint64_t value = 20151125;

  for (int i = 1; i < position; i++) {
    value *= 252533;
    value %= 33554393;
  }

  printf("%lu\n", value);

  return 0;
}
