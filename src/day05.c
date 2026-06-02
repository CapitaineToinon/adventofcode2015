#include "common.h"
#include <stdio.h>

#define VOWELS_COUNT 3

bool is_nice(char *s) {
  int i = 0;
  int vowels = 0;
  bool consecutive = false;

  while (s[i] != '\n') {
    int j = i + 1;

    if ((s[i] == 'a' && s[j] == 'b') || (s[i] == 'c' && s[j] == 'd') ||
        (s[i] == 'p' && s[j] == 'q') || (s[i] == 'x' && s[j] == 'y')) {
      return false;
    }

    if (vowels < VOWELS_COUNT && (s[i] == 'a' || s[i] == 'e' || s[i] == 'i' ||
                                  s[i] == 'o' || s[i] == 'u')) {
      vowels++;
    }

    if (!consecutive && s[i] == s[j]) {
      consecutive = true;
    }

    i++;
  }

  return consecutive && vowels >= VOWELS_COUNT;
}

bool is_very_nice(char *s) {
  int i = 0;
  bool has_pair = false;
  bool has_repeat = false;

  while (s[i + 2] != '\n') {
    if (!has_pair) {
      int j = i + 2;

      while (s[j] != '\n') {
        if (s[i] == s[j] && s[i + 1] == s[j + 1]) {
          has_pair = true;
          break;
        }

        j++;
      }
    }

    if (!has_repeat && s[i] == s[i + 2]) {
      has_repeat = true;
    }

    if (has_pair && has_repeat) {
      return true;
    }

    i++;
  }

  return false;
}

int main() {
  FILE *file = fopen_orexit("./input/day05");
  char line[256];
  int nice = 0;
  int very_nice = 0;

  while (fgets(line, sizeof(line), file)) {
    if (is_nice(line)) {
      nice++;
    }

    if (is_very_nice(line)) {
      very_nice++;
    }
  }

  printf("%d\n", nice);
  printf("%d\n", very_nice);
  fclose(file);
  return 0;
}
