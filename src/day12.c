#include <jq.h>
#include <jv.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAX 30000

regex_t regex;

int solve(char *line, int len);

int old() {
  char *line = malloc(sizeof(char) * MAX);
  FILE *file = fopen("./input/day12", "r");

  if (regcomp(&regex, "([0-9]+|-[0-9]+)", REG_EXTENDED) != 0) {
    printf("failed to compile regex\n");
    exit(-1);
  }

  regmatch_t matches[2];
  int result = 0;

  while (fgets(line, MAX, file)) {
    while (regexec(&regex, line, 2, matches, 0) == 0) {

      int number = atoi(line + matches[1].rm_so);
      result += number;
      printf("%d\n", number);

      line = line + matches[1].rm_eo;
    }
  }

  printf("%d\n", result);

  return 0;
}

int handle_object(char *line, int len) {
  bool valid = true;
  int end;

  for (end = 1;; end++) {
    if (strncmp(line + end, ":\"red\"", 6) == 0) {
      valid = false;
    }
    if (line[end] == '{') {
      handle_object(line + end, len);
    }
    if (line[end] == '}') {
      break;
    }
  }

  // printf("object found: %.*s and is %s\n", end + 1, line,
  //        valid ? "valid" : "invalid");

  if (!valid) {
    // printf("removing :%.*s\n", end - 1, line + 1);
    strncpy(line, line + end + 1, MAX);
    return solve(line, len - end + 1);
  } else {
    return solve(line + end + 1, len - end);
  }
}

int solve(char *line, int len) {
  int i = 0;

  while (i <= len) {
    // printf("i: %d len %d\n", i, len);

    if (line[i] == '{') {
      // printf("found object at %d -> %s\n", i, line + i);
      len = handle_object(line + i, len);
    }

    i++;
  }
}

int main() {
  char filename[] = "./input/day12";

  char *content = malloc(sizeof(char) * MAX);
  FILE *file = fopen(filename, "r");

  if (fread(content, sizeof(char), MAX, file) == 0) {
    perror("fread");
    exit(EXIT_FAILURE);
  }

  printf("%s\n", content);
  int len = strnlen(content, MAX);
  int new_len = solve(content, len);
  printf("%s\n", content);

  return 0;
}
