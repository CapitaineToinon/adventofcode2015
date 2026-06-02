#include "common.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

FILE *fopen_orexit(char *filename) {
  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    printf("failed to open file: %s\n", filename);
    exit(EXIT_FAILURE);
  }

  return file;
}

char *fopen_string_orexit(char *filename, int *out) {
  FILE *file = fopen_orexit(filename);

  struct stat sb;

  if (stat(filename, &sb) != 0) {
    printf("failed to open the file\n");
    exit(EXIT_FAILURE);
  }

  int size = sb.st_size;

  char *json = malloc(sizeof(char) * (size + 1));

  if (json == NULL) {
    printf("failed to malloc\n");
    exit(EXIT_FAILURE);
  }

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

void regcomp_orexit(regex_t *regex, char *input, int flags) {
  char buffer[100];
  int rc;

  if ((rc = regcomp(regex, input, flags)) != 0) {
    regerror(rc, regex, buffer, 100);
    printf("regcomp() failed with '%s'\n", buffer);
    exit(EXIT_FAILURE);
  }
}

int min2(int a, int b) { return a < b ? a : b; }
int min3(int a, int b, int c) { return min2(a, min2(b, c)); }

int max2(int a, int b) { return a > b ? a : b; }
