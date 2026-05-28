#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define N_EFFECT 5

typedef struct effect {
  int cost;
  int damage;
  int armor;
  int heal;
  int mana;
  int turns;
} effect_t;

typedef struct state {
  int phealth;
  int pmana;
  int used_mana;
  int bhealth;
  int bdamage;
  bool turn;
  effect_t **effects;
} state_t;

typedef struct node {
  state_t *state;
  struct node *next;
} node_t;

effect_t EFFECTS[N_EFFECT] = {
    {53, 4, 0, 0, 0, 0},    // Magic Missile
    {73, 2, 0, 2, 0, 0},    // Drain
    {113, 0, 7, 0, 0, 6},   // Shield
    {173, 3, 0, 0, 0, 6},   // Poison
    {229, 0, 0, 0, 101, 5}, // Recharge
};

char *EFFECT_NAMES[N_EFFECT] = {
    "Magic Missile", "Drain", "Shield", "Poison", "Recharge",
};

int min(int a, int b) { return a < b ? a : b; }
int max(int a, int b) { return a > b ? a : b; }

effect_t **create_effects() {
  effect_t **effects = malloc(sizeof(effect_t *) * N_EFFECT);

  for (int i = 0; i < N_EFFECT; i++) {
    effects[i] = NULL;
  }

  return effects;
}

void free_effects(effect_t **effects) {
  for (int i = 0; i < N_EFFECT; i++) {
    if (effects[i] != NULL) {
      free(effects[i]);
    }
  }

  free(effects);
}

effect_t *copy_effect(effect_t *e) {
  if (e == NULL) {
    return NULL;
  }

  effect_t *effect = malloc(sizeof(effect_t));
  effect->cost = e->cost;
  effect->damage = e->damage;
  effect->armor = e->armor;
  effect->heal = e->heal;
  effect->mana = e->mana;
  effect->turns = e->turns;
  return effect;
}

effect_t **copy_effects(effect_t **e) {
  effect_t **effects = malloc(sizeof(effect_t *) * N_EFFECT);

  for (int i = 0; i < N_EFFECT; i++) {
    effects[i] = copy_effect(e[i]);
  }

  return effects;
}

state_t *copy_state(state_t *state) {
  state_t *copy = malloc(sizeof(state_t));
  copy->phealth = state->phealth;
  copy->pmana = state->pmana;
  copy->used_mana = state->used_mana;
  copy->bhealth = state->bhealth;
  copy->bdamage = state->bdamage;
  copy->turn = state->turn;
  copy->effects = copy_effects(state->effects);
  return copy;
}

void free_state(state_t *state) {
  free_effects(state->effects);
  free(state);
}

void free_node(node_t *node) {
  if (node != NULL) {
    free_state(node->state);
    free_node(node->next);
    free(node);
  }
}

state_t *pop(node_t **q) {
  if (*q == NULL) {
    return NULL;
  }

  state_t *state = (*q)->state;
  node_t *old = *q;
  *q = (*q)->next;
  free(old);

  return state;
}

void insert(node_t **q, state_t *b) {
  if (*q == NULL) {
    node_t *root = malloc(sizeof(node_t));
    root->state = b;
    root->next = NULL;
    *q = root;
    return;
  }

  state_t *a = (*q)->state;

  if (a->used_mana > b->used_mana ||
      (a->used_mana == b->used_mana && a->bhealth > b->bhealth)) {
    node_t *insert = malloc(sizeof(node_t));
    insert->state = b;
    insert->next = *q;
    *q = insert;
    return;
  }

  insert(&(*q)->next, b);
}

int fight(int phealth, int pmana, int bhealth, int bdamage) {
  state_t *init = malloc(sizeof(state_t));
  init->phealth = phealth;
  init->pmana = pmana;
  init->used_mana = 0;
  init->bhealth = bhealth;
  init->bdamage = bdamage;
  init->turn = true;
  init->effects = create_effects();

  node_t *q = NULL;
  insert(&q, init);
  int solution = -1;

  while (q != NULL && solution == -1) {
    state_t *state = pop(&q);

    printf("%d\n", state->used_mana);

    if (state == NULL) {
      // no solution found
      break;
    }

    if (state->phealth <= 0) {
      // player died, dead branch
      free_state(state);
      continue;
    }

    if (state->bhealth <= 0) {
      solution = state->used_mana;
      free_state(state);
      break;
    }

    int armor = 0;

    for (int i = 0; i < N_EFFECT; i++) {
      effect_t *e = state->effects[i];

      if (e == NULL) {
        continue;
      }

      state->bhealth -= e->damage;
      state->phealth += e->heal;
      state->pmana += e->mana;
      armor += state->effects[i]->armor;

      e->turns--;

      if (e->turns <= 0) {
        free(e);
        state->effects[i] = NULL;
      }
    }

    if (state->bhealth <= 0) {
      solution = state->used_mana;
      free_state(state);
      break;
    }

    if (state->turn) {
      for (int i = 0; i < N_EFFECT; i++) {
        if (state->effects[i] != NULL) {
          // spell already casted
          continue;
        }

        effect_t *e = &EFFECTS[i];
        int cost = e->cost;

        // skip if not enough mana to cast the spell
        if (cost > state->pmana) {
          continue;
        }

        // casting effect
        state_t *next = copy_state(state);

        if (e->turns == 0) {
          // spell runs immiediately
          next->bhealth -= e->damage;
          next->phealth += e->heal;
          next->pmana += e->mana;
        } else {
          // spell is caster for later
          next->effects[i] = copy_effect(e);
        }

        // account for the cost of the spell
        next->pmana -= cost;
        next->used_mana += cost;

        if (next->bhealth <= 0) {
          solution = next->used_mana;
          free_state(next);
          break;
        }

        // change the turn
        next->turn = false;

        insert(&q, next);
      }

      free_state(state);
    } else {
      // Boss turn
      state->phealth -= max(1, state->bdamage - armor);
      state->turn = true;
      insert(&q, state);
    }
  }

  free_node(q);

  return solution;
}

int main() {
  effect_t **effects = create_effects();

  int part_1 = fight(50, 500, 55, 8);
  printf("%d\n", part_1);

  free_effects(effects);

  return 0;
}
