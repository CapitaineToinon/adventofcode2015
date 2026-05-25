#include <regex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RULES 100
#define MAX_LINE 1000
#define MAX_RULE_LEN 10
#define INPUT_REG_GROUP 2
#define RULE_REG_GROUP 3

// Note, all strings in these structs
// are not null terminalted, hence the <name>_len
// fields.

int min(int a, int b) { return a < b ? a : b; }
int max(int a, int b) { return a > b ? a : b; }

typedef struct rule {
  char *from;
  char *to;
  int from_len;
  int to_len;
} rule_t;

typedef struct context {
  rule_t **rules;
  int rules_len;
  char *input;
  int input_len;
} context_t;

typedef struct cache {
  uint64_t *hashes;
  int size;
} cache_t;

cache_t *create_cache() {
  cache_t *cache = malloc(sizeof(cache_t));
  cache->hashes = NULL;
  cache->size = 0;
  return cache;
}

void free_cache(cache_t *cache) {
  if (cache->hashes != NULL) {
    free(cache->hashes);
  }

  free(cache);
}

void shuffle(rule_t **rules, int n) {
  for (int i = 0; i < n - 1; i++) {
    size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
    rule_t *t = rules[j];
    rules[j] = rules[i];
    rules[i] = t;
  }
}

bool is_cached(uint64_t hash, cache_t *cache) {
  for (int i = 0; i < cache->size; i++) {
    if (cache->hashes[i] == hash) {
      return true;
    }
  }

  return false;
}

bool add_cache(uint64_t hash, cache_t *cache) {
  if (is_cached(hash, cache)) {
    return false;
  }

  cache->hashes = realloc(cache->hashes, sizeof(uint64_t) * (cache->size + 1));
  cache->hashes[cache->size] = hash;
  cache->size++;

  return true;
}

uint64_t djb2(char *input, int input_len) {
  uint64_t hash = 5381;

  for (int i = 0; i < input_len; i++) {
    hash = ((hash << 5) + hash) + input[i];
  }

  return hash;
}

void invert_rules(rule_t **rules, int rules_len) {
  for (int i = 0; i < rules_len; i++) {
    rule_t *r = rules[i];

    char *tmp = r->from;
    r->from = r->to;
    r->to = tmp;

    int tmp_len = r->from_len;
    r->from_len = r->to_len;
    r->to_len = tmp_len;
  }
}

char *regmatch_to_string(char *line, regmatch_t *m, int *output_len) {
  int len = m->rm_eo - m->rm_so;
  char *output = malloc(sizeof(char) * len);
  memcpy(output, line + m->rm_so, len);

  *output_len = len;
  return output;
}

context_t *get_context() {
  FILE *file = fopen("./input/day19", "r");

  char line[MAX_LINE];
  regex_t input_reg;
  regex_t rules_reg;

  if (regcomp(&input_reg, "^([a-zA-Z]+)\n$", REG_EXTENDED)) {
    printf("failed to compile regex\n");
    exit(-1);
  }

  if (regcomp(&rules_reg, "^([a-zA-Z]+) => ([a-zA-Z]+)\n$", REG_EXTENDED)) {
    printf("failed to compile regex\n");
    exit(-1);
  }

  context_t *ctx = malloc(sizeof(context_t));
  ctx->rules = NULL;
  ctx->rules_len = 0;

  while (fgets(line, MAX_LINE, file)) {
    regmatch_t rules_match[RULE_REG_GROUP];
    regmatch_t input_match[INPUT_REG_GROUP];

    if (regexec(&rules_reg, line, RULE_REG_GROUP, rules_match, 0) == 0) {
      rule_t *rule = malloc(sizeof(rule_t));
      rule->from = regmatch_to_string(line, &rules_match[1], &rule->from_len);
      rule->to = regmatch_to_string(line, &rules_match[2], &rule->to_len);
      ctx->rules = realloc(ctx->rules, sizeof(rule_t *) * (ctx->rules_len + 1));
      ctx->rules[ctx->rules_len++] = rule;
      continue;
    }

    if (regexec(&input_reg, line, INPUT_REG_GROUP, input_match, 0) == 0) {
      ctx->input = regmatch_to_string(line, &input_match[1], &ctx->input_len);
      continue;
    }
  }

  fclose(file);
  regfree(&rules_reg);
  regfree(&input_reg);

  return ctx;
}

void free_context(context_t *ctx) {
  for (int i = 0; i < ctx->rules_len; i++) {
    free(ctx->rules[i]->from);
    free(ctx->rules[i]->to);
    free(ctx->rules[i]);
  }

  free(ctx->rules);
  free(ctx->input);
  free(ctx);
}

char *apply_rule(char *input, int input_len, int pos, rule_t *rule,
                 int *output_len) {
  int next_len = input_len - rule->from_len + rule->to_len;
  char *next = malloc(next_len);

  memcpy(next, input, pos);
  memcpy(next + pos, rule->to, rule->to_len);
  memcpy(next + pos + rule->to_len, input + pos + rule->from_len,
         input_len - pos - rule->from_len);

  *output_len = next_len;
  return next;
}

char *copy_string(char *s, int len) {
  char *copy = malloc(sizeof(char) * len);
  memcpy(copy, s, len);
  return copy;
}

int solve(context_t *ctx) {
  cache_t *cache = create_cache();

  for (int i = 0; i < ctx->rules_len; i++) {
    rule_t *r = ctx->rules[i];
    for (int pos = 0; pos <= ctx->input_len - r->from_len; pos++) {
      if (memcmp(ctx->input + pos, r->from, r->from_len) == 0) {
        int next_len = 0;
        char *next = apply_rule(ctx->input, ctx->input_len, pos, r, &next_len);
        int hash = djb2(next, next_len);
        (void)add_cache(hash, cache);
        free(next);
      }
    }
  }

  int result = cache->size;
  free_cache(cache);
  return result;
}

int funny(context_t *ctx) {
  while (true) {
    int cur_len = ctx->input_len;
    char *cur = copy_string(ctx->input, ctx->input_len);
    int steps = 0;
    bool stuck = false;

    while (!(cur_len == 1 && cur[0] == 'e')) {
      bool found = false;
      for (int i = 0; i < ctx->rules_len && !found; i++) {
        rule_t *r = ctx->rules[i];
        for (int pos = 0; pos <= cur_len - r->from_len && !found; pos++) {
          if (memcmp(cur + pos, r->from, r->from_len) == 0) {
            char *next = apply_rule(cur, cur_len, pos, r, &cur_len);
            free(cur);
            steps++;
            cur = next;
            found = true;
          }
        }
      }

      if (!found) {
        stuck = true;
        break;
      }
    }

    if (!stuck) {
      free(cur);
      return steps;
    }

    // Try again
    free(cur);
    shuffle(ctx->rules, ctx->rules_len);
  }
}

int main() {
  context_t *ctx = get_context();

  int part_1 = solve(ctx);
  printf("%d\n", part_1);

  invert_rules(ctx->rules, ctx->rules_len);
  int part_2 = funny(ctx);
  printf("%d\n", part_2);

  free_context(ctx);

  return 0;
}
