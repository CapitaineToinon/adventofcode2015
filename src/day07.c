#include "common.h"
#include <regex.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_LEN 256
#define MAX_REGEX_LEN 256
#define CACHE_SIZE "aaa"

struct double_params {
  char *left;
  char *right;
  char *action;
};

struct context {
  FILE *file;
  int *cache;
  regex_t double_regex;
  regex_t not_regex;
  regex_t single_regex;
};

uint16_t solve(struct context *, char *);
int wire_hash(char *);
void init_cache(int *);
int *create_cache();

void init_cache(int *cache) {
  int size = wire_hash(CACHE_SIZE);
  for (int i = 0; i < size; i++) {
    cache[i] = -1;
  }
}

int *create_cache() {
  int size = wire_hash(CACHE_SIZE);
  int *cache = malloc(sizeof(int) * size);
  init_cache(cache);
  return cache;
}

void regex_compile(regex_t *regex, char *input) {
  regcomp_orexit(regex, input, REG_EXTENDED | REG_NEWLINE);
}

struct context *create_context() {
  struct context *ctx = malloc(sizeof(struct context));

  ctx->file = fopen_orexit("./input/day07");
  ctx->cache = create_cache();
  regex_compile(&ctx->double_regex, "(.+) (AND|OR|RSHIFT|LSHIFT) (.+)");
  regex_compile(&ctx->not_regex, "^NOT (.+)$");
  regex_compile(&ctx->single_regex, "^([a-z]+|[0-9]+)$");

  return ctx;
}

void free_context(struct context *ctx) {
  fclose(ctx->file);
  free(ctx->cache);
  regfree(&ctx->double_regex);
  regfree(&ctx->not_regex);
  regfree(&ctx->single_regex);
  free(ctx);
}

/**
 * Converts a wire to its position as an int so that
 * we don't need to implement a hashmap and just use
 * an array as a cache instead.
 */
int wire_hash(char *wire) {
  int length = strnlen(wire, 10);
  int key = 0;

  // offset: count all wires of shorter length (26 + 26^2 + ... + 26^(length-1))
  int p = 26;
  for (int i = 1; i < length; i++) {
    key += p;
    p *= 26;
  }

  // base-26 positional value, least significant first
  for (int i = 0; i < length; i++) {
    int value = wire[length - 1 - i] - 'a';

    // equivalent of powering 26
    p = 1;
    for (int j = 0; j < i; j++) {
      p *= 26;
    }
    key += value * p;
  }

  return key;
}

/**
 * Given an input and a regex match, copies the match
 * to a new string.
 */
char *extract_match(char *string, regmatch_t match) {
  int length = match.rm_eo - match.rm_so;
  char *output = malloc(sizeof(char) * (length + 1));
  strncpy(output, string + match.rm_so, length);
  output[length] = '\0';
  return output;
}

/**
 * Checks if the instruction is a single instruction (<wire|value>)
 * and if so, returns the given wire or value. Returns NULL otherwise.
 */
char *parse_single_params(struct context *ctx, char *instruction) {
  regmatch_t matches[2];
  char *wire = NULL;

  if (regexec(&ctx->single_regex, instruction,
              sizeof(matches) / sizeof(regmatch_t), matches, 0) == 0) {
    wire = extract_match(instruction, matches[1]);
  }

  return wire;
}

/**
 * Checks if the instruction is a not instruction (NOT <wire|value>)
 * and if so, returns the given wire or value. Returns NULL otherwise.
 */
char *parse_not_params(struct context *ctx, char *instruction) {
  regmatch_t matches[2];
  char *wire = NULL;

  if (regexec(&ctx->not_regex, instruction,
              sizeof(matches) / sizeof(regmatch_t), matches, 0) == 0) {
    wire = extract_match(instruction, matches[1]);
  }

  return wire;
}

/**
 * Checks if the instruction is a double instruction (<wire|value> <action>
 * <wire|value>) and if so, returns both wires and the action in a struct.
 * Returns NULL otherwise
 */
struct double_params *parse_double_params(struct context *ctx,
                                          char *instruction) {
  regmatch_t matches[4];
  struct double_params *params = NULL;

  if (regexec(&ctx->double_regex, instruction,
              sizeof(matches) / sizeof(regmatch_t), matches, 0) == 0) {
    params = malloc(sizeof(struct double_params));
    params->left = extract_match(instruction, matches[1]);
    params->action = extract_match(instruction, matches[2]);
    params->right = extract_match(instruction, matches[3]);
  }

  return params;
}

/**
 * Computes <value|wire> <action> <value|wire> -> <wire> by resolving
 * both left and right values and executing the given action on the
 * resolved values
 */
uint16_t solve_double(struct context *ctx, struct double_params *params) {
  uint16_t value_left = solve(ctx, params->left);
  uint16_t value_right = solve(ctx, params->right);

  if (strncmp(params->action, "AND", 3) == 0) {
    return value_left & value_right;
  }

  else if (strncmp(params->action, "OR", 2) == 0) {
    return value_left | value_right;
  }

  else if (strncmp(params->action, "RSHIFT", 6) == 0) {
    return value_left >> value_right;
  }

  else if (strncmp(params->action, "LSHIFT", 6) == 0) {
    return value_left << value_right;
  }

  printf("unknown action: %s\n", params->action);
  exit(EXIT_FAILURE);
}

/**
 * Computes NOT <value|wire> -> <wire> by resolving <value|wire>
 * and returning that resolved value bitwised NOT
 */
uint16_t solve_not(struct context *ctx, char *wire) {
  return ~solve(ctx, wire);
}

/**
 * Computes <value|wire> -> <wire> by resolving <value|wire>
 */
uint16_t solve_single(struct context *ctx, char *wire) {
  return solve(ctx, wire);
}

uint16_t solve(struct context *ctx, char *wire) {
  // If the wire starts with a number, we're dealing
  // with a hardcoded value, early return
  if ('0' <= wire[0] && wire[0] <= '9') {
    return (uint16_t)atoi(wire);
  }

  // Implement a cache to avoid recomputing
  // already computed wires
  int key = wire_hash(wire);
  if (ctx->cache[key] != -1) {
    return (uint16_t)ctx->cache[key];
  }

  char *line = malloc(sizeof(char) * LINE_LEN);
  char input[MAX_REGEX_LEN];

  // Regex used to find the line that assigns the value
  // to the wire we're looking for. By design, there is
  // only one in the file
  if (sprintf(input, "^(.+) -> %s$", wire) < 0) {
    printf("failed to sprintf\n");
    exit(EXIT_FAILURE);
  }

  regex_t regex;
  regmatch_t matches[2];
  regex_compile(&regex, input);

  // Ensure we move back at the start of the file before searching
  // for our wire. Need to do this since we recursively call solve, moving
  // the file handle
  fseek(ctx->file, 0, SEEK_SET);

  while (fgets(line, LINE_LEN, ctx->file)) {
    if (regexec(&regex, line, sizeof(matches) / sizeof(regmatch_t), matches,
                0) != 0) {
      continue;
    }

    char *instruction = extract_match(line, matches[1]);

    uint16_t result;
    struct double_params *d_params = NULL;
    char *w = NULL;

    if ((d_params = parse_double_params(ctx, instruction)) != NULL) {
      result = solve_double(ctx, d_params);
      free(d_params->left);
      free(d_params->right);
      free(d_params->action);
      free(d_params);
    }

    else if ((w = parse_not_params(ctx, instruction)) != NULL) {
      result = solve_not(ctx, w);
      free(w);
    }

    else if ((w = parse_single_params(ctx, instruction)) != NULL) {
      result = solve_single(ctx, w);
      free(w);
    }

    else {
      printf("found instruction but failed to parsed it: %s\n", instruction);
      exit(EXIT_FAILURE);
    }

    free(instruction);
    free(line);
    regfree(&regex);

    // cache for memoization
    ctx->cache[key] = (int)result;
    return result;
  }

  printf("failed to process wire %s\n", wire);
  exit(EXIT_FAILURE);
}

int main() {
  struct context *ctx = create_context();

  uint16_t part_1 = solve(ctx, "a");
  printf("%d\n", part_1);

  // reset cache and for b to have the output
  // of part 1 to solve part 2
  init_cache(ctx->cache);
  int key_b = wire_hash("b");
  ctx->cache[key_b] = (uint16_t)part_1;

  uint16_t part_2 = solve(ctx, "a");
  printf("%d\n", part_2);

  free_context(ctx);
  return 0;
}
