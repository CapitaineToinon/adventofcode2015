#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

char *get_input(const char *filename, int *out) {
  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    printf("failed to open the file\n");
    exit(EXIT_FAILURE);
  }

  struct stat sb;

  if (stat(filename, &sb) != 0) {
    printf("failed to open the file\n");
    exit(EXIT_FAILURE);
  }

  int size = sb.st_size;
  char *json = malloc(sizeof(char) * (size + 1));
  int read = fread(json, sizeof(char), size, file);

  if (read != size) {
    printf("failed to read file, size mismatch\n");
    exit(EXIT_FAILURE);
  }

  fclose(file);

  json[size] = '\0';
  *out = size;

  return json;
}

int part_1(char *input) {
  regex_t regex;

  if (regcomp(&regex, "([0-9]+|-[0-9]+)", REG_EXTENDED) != 0) {
    printf("failed to compile regex\n");
    exit(-1);
  }

  regmatch_t matches[2];
  int result = 0;

  while (regexec(&regex, input, 2, matches, 0) == 0) {
    result += atoi(input + matches[1].rm_so);
    input += matches[1].rm_eo;
  }

  regfree(&regex);

  return result;
}

int find_start(char *cur, int i) {
  int depth = 0;

  while (i > 0) {
    if (cur[i] == '}') {
      depth++;
    } else if (cur[i] == '{') {
      if (depth == 0) {
        return i;
      }
      depth--;
    }

    i--;
  }

  printf("failed to find the start of the object\n");
  exit(EXIT_FAILURE);
}

int find_end(char *cur, int i) {
  int depth = 0;

  while (cur[i] != '\0') {
    if (cur[i] == '{') {
      depth++;
    } else if (cur[i] == '}') {
      if (depth == 0) {
        return i + 1;
      }
      depth--;
    }

    i++;
  }

  printf("failed to find the end of the object\n");
  exit(EXIT_FAILURE);
}

int part_2(char *input, int len) {
  regex_t regex;
  int rc;

  if ((rc = regcomp(&regex, "\"[a-z]+\":\"red\"", REG_EXTENDED)) != 0) {
    char buffer[100];
    regerror(rc, &regex, buffer, 100);
    printf("regcomp() failed with '%s'\n", buffer);
    exit(1);
  }

  regmatch_t matches[1];

  char *cur = malloc(sizeof(char) * (len + 1));
  memcpy(cur, input, len);
  cur[len] = '\0';

  char *next = malloc(sizeof(char) * (len + 1));

  while (regexec(&regex, cur, 1, matches, 0) == 0) {
    int start = find_start(cur, matches[0].rm_so);
    int end = find_end(cur, matches[0].rm_eo);
    int segment_len = end - start;

    memcpy(next, cur, start);
    memcpy(next + start + 1, cur + end, len - end);
    len -= (segment_len - 1);
    next[start] = '0'; // replace the object by a zero
    next[len] = '\0';

    // swap the buffers
    char *tmp = cur;
    cur = next;
    next = tmp;
  }

  int result = part_1(cur);

  free(cur);
  free(next);
  regfree(&regex);

  return result;
}

int main() {
  int len;
  char *json = get_input("./input/day12", &len);

  printf("%d\n", part_1(json));
  printf("%d\n", part_2(json, len));

  free(json);
  return 0;
}
