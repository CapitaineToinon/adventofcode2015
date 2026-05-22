#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RULES 100
#define MAX_LINE 1000
#define MAX_RULE_LEN 10

// Note, all strings in these structs
// are not null terminalted, hence the <name>_len
// fields.

typedef struct rule {
  char *from;
  char *to;
  int from_len;
  int to_len;
} rule_t;

typedef struct context {
  rule_t *rules;
  int rules_len;
  char *input;
  int input_len;
} context_t;

typedef struct cache {
  int *hashes;
  int size;
} cache_t;

cache_t *create_cache() {
  cache_t *cache = malloc(sizeof(cache_t));
  cache->hashes = NULL;
  cache->size = 0;
  return cache;
}

bool is_cached(int hash, cache_t *cache) {
  for (int i = 0; i < cache->size; i++) {
    if (cache->hashes[i] == hash) {
      return true;
    }
  }

  return false;
}

bool add_cache(int hash, cache_t *cache) {
  if (is_cached(hash, cache)) {
    return false;
  }

  cache->hashes = realloc(cache->hashes, sizeof(int) * (cache->size + 1));
  cache->hashes[cache->size] = hash;
  cache->size++;

  return true;
}

int djb2(char *input, int input_len) {
  int hash = 5381;

  for (int i = 0; i < input_len; i++) {
    hash = ((hash << 5) + hash) + input[i];
  }

  return hash;
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
    regmatch_t rules_match[3];
    regmatch_t input_match[2];

    if (regexec(&rules_reg, line, 3, rules_match, 0) == 0) {
      int from_len = rules_match[1].rm_eo - rules_match[1].rm_so;
      int to_len = rules_match[2].rm_eo - rules_match[2].rm_so;

      ctx->rules = realloc(ctx->rules, sizeof(rule_t) * (ctx->rules_len + 1));
      ctx->rules[ctx->rules_len].from = malloc(sizeof(char) * from_len);
      ctx->rules[ctx->rules_len].to = malloc(sizeof(char) * to_len);
      memcpy(ctx->rules[ctx->rules_len].from, line + rules_match[1].rm_so,
             from_len);
      memcpy(ctx->rules[ctx->rules_len].to, line + rules_match[2].rm_so,
             to_len);
      ctx->rules[ctx->rules_len].from_len = from_len;
      ctx->rules[ctx->rules_len].to_len = to_len;
      ctx->rules_len++;
      continue;
    }

    if (regexec(&input_reg, line, 2, input_match, 0) == 0) {
      int input_len = input_match[1].rm_eo - input_match[1].rm_so;
      ctx->input_len = input_len;
      ctx->input = malloc(sizeof(char) * ctx->input_len);
      memcpy(ctx->input, line + input_match[1].rm_so, ctx->input_len);
      continue;
    }
  }

  return ctx;
}

void print_context(context_t *ctx) {
  for (int i = 0; i < ctx->rules_len; i++) {
    printf("%.*s -> %.*s\n", ctx->rules[i].from_len, ctx->rules[i].from,
           ctx->rules[i].to_len, ctx->rules[i].to);
  }

  printf("\n%.*s (%d)\n", ctx->input_len, ctx->input, ctx->input_len);
}

bool input_match(char *input, rule_t *rule) {
  return memcmp(input, rule->from, rule->from_len) == 0;
}

char *apply_rule(char *input, int input_len, int pos, rule_t *rule,
                 int *output_len) {
  int next_len = input_len - rule->from_len + rule->to_len;
  char *next = malloc(next_len);

  if (next == NULL) {
    printf("failed to malloc\n");
    exit(EXIT_FAILURE);
  }

  memcpy(next, input, pos);
  memcpy(next + pos, rule->to, rule->to_len);
  memcpy(next + pos + rule->to_len, input + pos + rule->from_len,
         input_len - pos - 1);

  *output_len = next_len;
  return next;
}

int solve(char *input, int input_len, rule_t *rules, int rules_len) {
  cache_t *cache = create_cache();

  for (int pos = 0; pos < input_len; pos++) {
    for (int r = 0; r < rules_len; r++) {
      if (input_match(input + pos, &rules[r])) {
        int next_len = 0;
        char *next = apply_rule(input, input_len, pos, &rules[r], &next_len);
        int hash = djb2(next, next_len);
        (void)add_cache(hash, cache);
      }
    }
  }

  return cache->size;
}

int solve_p2(char *input, int input_len, int target_hash, int target_len,
             int steps, rule_t *rules, int rules_len, cache_t *cache) {

  if (input_len > target_len) {
    return -1;
  }

  int current_hash = djb2(input, input_len);

  if (current_hash == target_hash) {
    return steps;
  }

  if (add_cache(current_hash, cache) == false) {
    // already tries this input
    return -1;
  }

  for (int pos = 0; pos < input_len; pos++) {
    for (int r = 0; r < rules_len; r++) {
      if (input_match(input + pos, &rules[r])) {
        // printf("found match for %.*s -> %.*s as pos %d\n", rules[r].from_len,
        //        input + pos, rules[r].to_len, rules[r].to, pos);
        int next_len = 0;
        char *next = apply_rule(input, input_len, pos, &rules[r], &next_len);

        // printf("next: %.*s\n", next_len, next);

        int solution = solve_p2(next, next_len, target_hash, target_len,
                                steps + 1, rules, rules_len, cache);

        free(next);

        if (solution != -1) {
          return solution;
        }
      }
    }
  }

  return -1;
}

int main() {
  context_t *ctx = get_context();
  print_context(ctx);

  {
    int part_1 = solve(ctx->input, ctx->input_len, ctx->rules, ctx->rules_len);
    printf("%d\n", part_1);
  }

  {
    cache_t *cache = create_cache();
    int target_hash = djb2(ctx->input, ctx->input_len);
    char *start = "e";
    int start_len = 1;

    int part_2 = solve_p2(start, start_len, target_hash, ctx->input_len, 0,
                          ctx->rules, ctx->rules_len, cache);

    printf("%d\n", part_2);
  }

  return 0;
}
