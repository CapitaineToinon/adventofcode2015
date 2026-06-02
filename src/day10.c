#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 10000000

char *solve(char *from, int depth) {
  char *seq = malloc(sizeof(char) * MAX_LEN);
  char *next = malloc(sizeof(char) * MAX_LEN);

  int seq_len = strnlen(from, MAX_LEN);
  int next_len;

  memcpy(seq, from, seq_len);

  while (depth--) {
    next_len = 0;

    int repeat_start = 0;
    int repeat_end = 0;

    while (repeat_end < seq_len) {
      char c = seq[repeat_end];

      while (seq[repeat_end] == c) {
        // counting repeating characters
        repeat_end++;
      }

      // code assumes never more than 9 repeating characters
      next[next_len++] = '0' + (repeat_end - repeat_start);
      next[next_len++] = c;
      repeat_start = repeat_end;
    }

    // swap seq and next
    char *tmp = seq;
    seq = next;
    next = tmp;
    seq_len = next_len;
  }

  free(next);

  return seq;
}

int main() {
  char input[] = "3113322113";
  char *part_1 = solve(input, 40);
  char *part_2 = solve(part_1, 10);

  printf("%ld\n", strnlen(part_1, MAX_LEN));
  printf("%ld\n", strnlen(part_2, MAX_LEN));

  free(part_1);
  free(part_2);

  return 0;
}
