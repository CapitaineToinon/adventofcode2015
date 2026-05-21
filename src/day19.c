#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct replacement {
  char *from;
  char *to;
  struct replacement *next;
} replacement_t;

typedef struct pattern {
  char *from;
  struct pattern *to;
  struct pattern *next;
} pattern_t;

/**
 * Creates a linked list of unprocessed patterns, simply
 * a list of match -> replacement
 */
void add_replacement(char *from, char *to, replacement_t **head,
                     replacement_t **tail) {
  replacement_t *next = malloc(sizeof(replacement_t));
  next->from = from;
  next->to = to;

  // This is the first replacement
  // so update the head
  if (*head == NULL) {
    *head = next;
    *tail = next;
  }

  // Append to the tail
  // and update the tail
  (*tail)->next = next;
  (*tail) = (*tail)->next;
}

void create_patterns_froms_only(replacement_t *replacement, pattern_t **head) {
  // if NULL, we're done processing all replacements
  // and the patterns linked list is ready
  if (replacement == NULL) {
    return;
  }

  // if this is the first pattern we're creating
  // fill it with the root from, leave the to
  // for later
  if (*head == NULL) {
    (*head) = malloc(sizeof(pattern_t));
    (*head)->from = replacement->from;
  }

  // Now try to find the replacement->from in the
  // patterns as it may already exist, for example
  // if we add H -> HO but a previous H -> OH replacement
  // was already process, meaning H already exists
  pattern_t *pat = NULL;
  pattern_t *tail = (*head);

  while (true) {
    if (strcmp(replacement->from, tail->from) == 0) {
      printf("%s already exists, add new to: %s\n", replacement->from,
             replacement->to);
      pat = tail;
      break;
    }

    // Do it this way to have the correct
    // tail in case we don't find the replacement
    // in the existing patterns
    if (tail->next == NULL) {
      break;
    }

    tail = tail->next;
  }

  // If pat is NULL, this is the first time we see
  // a replacement from <X> -> <anything> so create the structure
  if (pat == NULL) {
    printf("%s seen for the first time, add new to %s\n", replacement->from,
           replacement->to);
    pat = malloc(sizeof(pattern_t));
    pat->from = replacement->from;

    // also chain it at the end of existing patterns
    // so that next calls to this function can find it
    // in case we have a new replacement that reuses
    // whatever <X> was in <X> -> anything
    tail->next = pat;
  }

  // Now split the string of "to" into patterns
  // and creates theses if needed

  // Done processing the replacement,
  // move to the next one
  create_patterns_froms_only(replacement->next, head);
}

void patterns_fill_tos(replacement_t *replacement, pattern_t *head) {
  // We're done processing all
  // the replacements
  if (replacement == NULL) {
    return;
  }
}

void print_patterns(pattern_t *patterns) {
  if (patterns == NULL) {
    return;
  }

  printf("%s:\n", patterns->from);

  pattern_t *to = patterns->to;

  while (to) {
    printf("-> %s\n", to->from);
    to = to->next;
  }

  print_patterns(patterns->next);
}

void print_replacements(replacement_t *replacement) {
  if (replacement == NULL) {
    return;
  }

  printf("%s -> %s\n", replacement->from, replacement->to);
  print_replacements(replacement->next);
}

int main() {
  replacement_t *head = NULL;
  replacement_t *tail = NULL;

  add_replacement("H", "HO", &head, &tail);
  add_replacement("H", "OH", &head, &tail);
  add_replacement("O", "HH", &head, &tail);

  print_replacements(head);

  pattern_t *patterns = NULL;
  create_patterns_froms_only(head, &patterns);
  patterns_fill_tos(head, patterns);

  print_patterns(patterns);

  // pattern_t *patterns = replacements_to_patterns(replacements, NULL);
  // print_patterns(patterns);

  return 0;
}
