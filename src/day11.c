#include <stdio.h>

#define PASSWORD_LEN 8

void increase(char *p) {
  int i = PASSWORD_LEN - 1;
  bool carry = false;

  do {
    carry = p[i] == 'z';
    p[i] = 'a' + (((p[i] - 'a') + 1) % 26);
    i--;
  } while (carry && i >= 0);
}

bool is_valid(char *p) {
  bool has_increase = false;
  int first_repeat = -1;
  int second_repeat = -1;

  for (int i = 0; i < PASSWORD_LEN; i++) {
    if (p[i] == 'i' || p[i] == 'o' || p[i] == 'l') {
      return false;
    }

    if (first_repeat == -1 && p[i] == p[i + 1]) {
      first_repeat = i;
    }

    if (second_repeat == -1 && first_repeat != -1 && i - first_repeat >= 2 &&
        p[i] == p[i + 1]) {
      second_repeat = i;
    }

    if (!has_increase && p[i] == p[i + 1] - 1 && p[i] == p[i + 2] - 2) {
      has_increase = true;
    }
  }

  return has_increase && first_repeat != -1 && second_repeat != -1;
}

void solve(char *p) {
  do {
    increase(p);
  } while (!is_valid(p));
}

int main() {
  char password[] = "vzbxkghb";

  solve(password);
  printf("%s\n", password);
  solve(password);
  printf("%s\n", password);

  return 0;
}
