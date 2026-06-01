#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 10
#define MAX_PACKAGES 64

typedef struct result {
  int size;
  uint64_t state;
  uint64_t qe;
} result_t;

int set_state(uint64_t state, int i) { return (state | (1 << i)); }
int get_state(uint64_t state, int i) { return (state >> i) & 1; }

int sum(int *packages, int n) {
  int total = 0;

  for (int i = 0; i < n; i++) {
    total += packages[i];
  }

  return total;
}

result_t *update_output(result_t *output, result_t *next) {
  if (next == NULL) {
    return output;
  }

  if (output == NULL) {
    return next;
  }

  if (next->size < output->size ||
      (next->size == output->size && next->qe < output->qe)) {
    free(output);
    return next;
  }

  free(next);

  return output;
}

result_t *create_group(int *packages, uint64_t global_state, uint64_t state,
                       int n, int target, int i, int weight, int size,
                       uint64_t qe) {
  if (weight == target) {
    result_t *r = malloc(sizeof(result_t));
    r->state = state;
    r->size = size;
    r->qe = qe;
    return r;
  }

  if (i >= n) {
    return NULL;
  }

  result_t *output = NULL;

  if (get_state(global_state, i) == 0) {
    output = update_output(output, create_group(packages, global_state,
                                                set_state(state, i), n, target,
                                                i + 1, weight + packages[i],
                                                size + 1, qe * packages[i]));
  }

  output = update_output(output, create_group(packages, global_state, state, n,
                                              target, i + 1, weight, size, qe));

  return output;
}

uint64_t solve(int *packages, int n, int n_groups) {
  if (n > MAX_PACKAGES) {
    printf("cannot have more than %d packages\n", MAX_PACKAGES);
    exit(-1);
  }

  int total = sum(packages, n);

  if (total % n_groups != 0) {
    printf("impossible to divide packages in %d\n", n_groups);
    exit(-1);
  }

  int target = total / n_groups;
  result_t **groups = malloc(sizeof(result_t *) * n_groups);
  uint64_t global_state = 0;

  for (int i = 0; i < n_groups; i++) {
    groups[i] = create_group(packages, global_state, 0, n, target, 0, 0, 0, 1);
    global_state |= groups[i]->state;
  }

  uint64_t result = groups[0]->qe;

  for (int i = 0; i < n_groups; i++) {
    free(groups[i]);
  }

  free(groups);

  return result;
}

int main() {
  FILE *file = fopen("./input/day24", "r");
  char line[MAX_LINE];

  int *packages = malloc(sizeof(int) * MAX_PACKAGES);
  int n = 0;

  while (fgets(line, MAX_LINE, file)) {
    packages[n++] = atoi(line);
  }

  fclose(file);

  printf("%lu\n", solve(packages, n, 3));
  printf("%lu\n", solve(packages, n, 4));

  free(packages);

  return 0;
}
