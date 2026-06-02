#include "common.h"
#include <regex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256
#define N_INSTRUCTIONS 6
#define N_REGISTERS 2

typedef struct raw_instruction {
  char *input;
  int n_groups;
} raw_instruction_t;

typedef struct regex_instruction {
  int type;
  regex_t regex;
  regmatch_t *matches;
  int n_groups;
} regex_instruction_t;

typedef struct instruction {
  int type;
  void *params;
} instruction_t;

typedef struct conditional_jump {
  int reg;
  int offset;
} conditional_jump_t;

typedef struct state {
  uint32_t instruction_pointer;
  uint32_t *registers;
  instruction_t **instructions;
  uint32_t n;
} state_t;

typedef void *(*Parser)(char *, regmatch_t *);
typedef void (*Executer)(state_t *, instruction_t *);

char *regmatch_to_string(char *line, regmatch_t *m) {
  int len = m->rm_eo - m->rm_so;
  char *output = malloc(sizeof(char) * (len + 1));
  memcpy(output, line + m->rm_so, len);
  output[len] = '\0';
  return output;
}

char regmatch_to_char(char *line, regmatch_t *m) {
  char *s = regmatch_to_string(line, m);
  int len = strnlen(s, MAX_LINE);

  if (len != 1) {
    printf("Expected to match a single char, matched more\n");
    exit(EXIT_FAILURE);
  }

  char output = s[0];
  free(s);
  return output;
}

int regmatch_to_offset(char *line, regmatch_t *m) {
  char *s = regmatch_to_string(line, m);
  int output = atoi(s);
  free(s);
  return output;
}

int regmatch_to_register(char *line, regmatch_t *m,
                         const char registers[N_REGISTERS]) {
  char reg = regmatch_to_char(line, m);

  for (int i = 0; i < N_REGISTERS; i++) {
    if (registers[i] == reg) {
      return i;
    }
  }

  printf("Register %c does not exist\n", reg);
  exit(EXIT_FAILURE);
}

const raw_instruction_t RAW_INSTRUCTIONS[N_INSTRUCTIONS] = {
    {
        "^hlf ([a-z])\n$",
        2,
    },
    {
        "^tpl ([a-z])\n$",
        2,
    },
    {
        "^inc ([a-z])\n$",
        2,
    },
    {
        "^jmp (\\+[0-9]+|-[0-9]+)\n$",
        2,
    },
    {
        "^jie ([a-z]), (\\+[0-9]+|-[0-9]+)\n$",
        3,
    },
    {
        "^jio ([a-z]), (\\+[0-9]+|-[0-9]+)\n$",
        3,
    }};

const char RAW_REGISTERS[N_REGISTERS] = {
    'a',
    'b',
};

void *parse_register(char *line, regmatch_t *matches) {
  int *reg = malloc(sizeof(int));
  *reg = regmatch_to_register(line, &matches[1], RAW_REGISTERS);
  return (void *)reg;
}

void *parse_offset(char *line, regmatch_t *matches) {
  int *offset = malloc(sizeof(int));
  *offset = regmatch_to_offset(line, &matches[1]);
  return (void *)offset;
}

void *parse_conditional_jump(char *line, regmatch_t *matches) {
  conditional_jump_t *params = malloc(sizeof(conditional_jump_t));
  params->reg = regmatch_to_register(line, &matches[1], RAW_REGISTERS);
  params->offset = regmatch_to_offset(line, &matches[2]);
  return (void *)params;
}

void execute_half(state_t *s, instruction_t *i) {
  int *reg = (int *)i->params;
  s->registers[*reg] /= 2;
  s->instruction_pointer++;
}

void execute_triple(state_t *s, instruction_t *i) {
  int *reg = (int *)i->params;
  s->registers[*reg] *= 3;
  s->instruction_pointer++;
}

void execute_increment(state_t *s, instruction_t *i) {
  int *reg = (int *)i->params;
  s->registers[*reg] += 1;
  s->instruction_pointer++;
}

void execute_jump(state_t *s, instruction_t *i) {
  int *offset = (int *)i->params;
  s->instruction_pointer += *offset;
}

void execute_jump_even(state_t *s, instruction_t *i) {
  conditional_jump_t *p = (conditional_jump_t *)i->params;

  if (s->registers[p->reg] % 2 == 0) {
    s->instruction_pointer += p->offset;
  } else {
    s->instruction_pointer++;
  }
}

void execute_jump_one(state_t *s, instruction_t *i) {
  conditional_jump_t *p = (conditional_jump_t *)i->params;

  if (s->registers[p->reg] == 1) {
    s->instruction_pointer += p->offset;
  } else {
    s->instruction_pointer++;
  }
}

const Parser PARSERS[N_INSTRUCTIONS] = {
    &parse_register, &parse_register,         &parse_register,
    &parse_offset,   &parse_conditional_jump, &parse_conditional_jump,
};

const Executer EXECUTERS[N_INSTRUCTIONS] = {
    &execute_half, &execute_triple,    &execute_increment,
    &execute_jump, &execute_jump_even, &execute_jump_one,
};

regex_instruction_t **
compile_raw_instructions(const raw_instruction_t raw[N_INSTRUCTIONS]) {
  regex_instruction_t **compiled =
      malloc(sizeof(regex_instruction_t *) * N_INSTRUCTIONS);

  for (int i = 0; i < N_INSTRUCTIONS; i++) {
    compiled[i] = malloc(sizeof(regex_instruction_t));
    compiled[i]->type = i;

    regcomp_orexit(&compiled[i]->regex, raw[i].input,
                   REG_NEWLINE | REG_EXTENDED);

    compiled[i]->matches = malloc(sizeof(regmatch_t) * raw[i].n_groups);
    compiled[i]->n_groups = raw[i].n_groups;
  }

  return compiled;
}

void free_regex_instructions(regex_instruction_t **regexes) {
  for (int i = 0; i < N_INSTRUCTIONS; i++) {
    regfree(&regexes[i]->regex);
    free(regexes[i]->matches);
    free(regexes[i]);
  }

  free(regexes);
}

instruction_t *match_line(char *line, regex_instruction_t **regexes) {
  for (int i = 0; i < N_INSTRUCTIONS; i++) {
    regex_instruction_t *r = regexes[i];

    if (regexec(&r->regex, line, r->n_groups, r->matches, 0) == 0) {
      Parser p = PARSERS[i];
      instruction_t *instruction = malloc(sizeof(instruction_t));
      instruction->type = r->type;
      instruction->params = p(line, r->matches);
      return instruction;
    }
  }

  printf("invalid instruction %s", line);
  exit(EXIT_FAILURE);
}

state_t *create_state() {
  state_t *state = malloc(sizeof(state_t));
  state->instruction_pointer = 0;
  state->instructions = NULL;
  state->n = 0;
  state->registers = calloc(N_REGISTERS, sizeof(uint32_t));
  return state;
}

void free_instruction(instruction_t *i) {
  free(i->params);
  free(i);
}

void free_state(state_t *state) {
  if (state->instructions != NULL) {
    for (uint32_t i = 0; i < state->n; i++) {
      free_instruction(state->instructions[i]);
    }

    free(state->instructions);
  }

  free(state->registers);
  free(state);
}

void execute(state_t *state) {
  while (state->instruction_pointer < state->n) {
    instruction_t *i = state->instructions[state->instruction_pointer];
    EXECUTERS[i->type](state, i);
  }
}

void reset(state_t *state) {
  state->instruction_pointer = 0;

  for (int i = 0; i < N_REGISTERS; i++) {
    state->registers[i] = 0;
  }
}

void add_instuction(state_t *s, instruction_t *i) {
  s->instructions =
      realloc(s->instructions, sizeof(instruction_t *) * (s->n + 1));
  s->instructions[s->n] = i;
  s->n++;
}

int main() {
  FILE *file = fopen_orexit("./input/day23");
  char line[MAX_LINE];

  regex_instruction_t **regexes = compile_raw_instructions(RAW_INSTRUCTIONS);
  state_t *state = create_state();

  while (fgets(line, MAX_LINE, file)) {
    add_instuction(state, match_line(line, regexes));
  }

  free_regex_instructions(regexes);
  fclose(file);

  execute(state);
  int part_1 = state->registers[1];

  reset(state);
  state->registers[0] = 1;

  execute(state);
  int part_2 = state->registers[1];

  free_state(state);

  printf("%d\n", part_1);
  printf("%d\n", part_2);

  return 0;
}
