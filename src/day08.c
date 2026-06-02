#include "common.h"
#include <stdio.h>

#define LINE_LEN 256

enum states {
  START,
  INSIDE_UNKNOWN,
  INSIDE_ESCAPE,
  END,
};

int decode(char *l) {
  int i = 0;
  int len = 0;
  enum states state = START;

  while (true) {
    switch (state) {
    case START:
      i++;
      state = INSIDE_UNKNOWN;
      break;
    case INSIDE_UNKNOWN:
      switch (l[i]) {
      case '\\':
        i++;
        state = INSIDE_ESCAPE;
        break;
      case '"':
        i++;
        state = END;
        break;
      default:
        i++;
        len++;
        break;
      }
      break;
    case INSIDE_ESCAPE:
      switch (l[i]) {
      case '"':
      case '\\':
        i++;
        len++;
        state = INSIDE_UNKNOWN;
        break;
      default:
        i += 3;
        len++;
        state = INSIDE_UNKNOWN;
        break;
      }
      break;
    case END:
      return i - len;
    }
  }
}

int encode(char *l) {
  int i = 0;
  int len = 2;

  while (l[i] != '\n') {
    switch (l[i]) {
    case '"':
    case '\\':
      len++;
    }

    i++;
  }

  return len;
}

int main() {
  char line[LINE_LEN];

  FILE *file = fopen_orexit("./input/day08");
  int part_1 = 0;
  int part_2 = 0;

  while (fgets(line, LINE_LEN, file)) {
    part_1 += decode(line);
    part_2 += encode(line);
  }

  printf("%d\n", part_1);
  printf("%d\n", part_2);

  return 0;
};
