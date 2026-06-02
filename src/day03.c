#include "common.h"
#include <stdlib.h>

#define WIDTH 1000
#define HEIGHT 1000

bool *create_map() {
  bool *map = malloc(sizeof(bool) * WIDTH * HEIGHT);
  return map;
}

int positive_mod(int i, int n) { return (i % n + n) % n; }

bool get(bool *map, int x, int y) {
  int real_x = positive_mod(x, WIDTH);
  int real_y = positive_mod(y, HEIGHT);
  return map[(real_y * WIDTH) + real_x];
}

void set(bool *map, int x, int y, bool value) {
  int real_x = positive_mod(x, WIDTH);
  int real_y = positive_mod(y, HEIGHT);
  map[(real_y * WIDTH) + real_x] = value;
}

void update_houses(bool *map, int x, int y, int *total) {
  int visited = get(map, x, y);

  if (!visited) {
    (*total)++;
  }

  set(map, x, y, true);
}

int main() {
  FILE *file = fopen_orexit("./input/day03");
  bool *p1_map = create_map();
  bool *p2_map = create_map();

  int i = 0;
  int p1_x = 0, p1_y = 0;
  int p2_santa_x = 0, p2_santa_y = 0;
  int p2_robot_x = 0, p2_robot_y = 0;
  int p1_houses = 0, p2_houses = 0;

  while (true) {
    char ch = fgetc(file);

    if (ch == EOF) {
      break;
    }

    // choses who takes turn for p2
    int *p2_x = i % 2 == 0 ? &p2_santa_x : &p2_robot_x;
    int *p2_y = i % 2 == 0 ? &p2_santa_y : &p2_robot_y;

    // update the map
    update_houses(p1_map, p1_x, p1_y, &p1_houses);
    update_houses(p2_map, *p2_x, *p2_y, &p2_houses);

    // update the positions
    if (ch == '>') {
      p1_x++;
      (*p2_x)++;
    }

    if (ch == '<') {
      p1_x--;
      (*p2_x)--;
    }

    if (ch == '^') {
      p1_y--;
      (*p2_y)--;
    }

    if (ch == 'v') {
      p1_y++;
      (*p2_y)++;
    }

    i++;
  }

  printf("%d\n", p1_houses);
  printf("%d\n", p2_houses);

  free(p1_map);
  free(p2_map);
  fclose(file);

  return 0;
}
