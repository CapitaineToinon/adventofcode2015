#include <stdio.h>
#include <stdlib.h>

#define N_WEAPON 5
#define N_ARMOR (5 + 1)
#define N_RINGS (6 + 1)

#define SET_ITEMS(items, values, N)                                            \
  do {                                                                         \
    int i, j;                                                                  \
    for (i = 0, j = 0; j < N; i += 3, j++) {                                   \
      set_item(values[i], values[i + 1], values[i + 2], items[j]);             \
    }                                                                          \
  } while (false)

#define PRINT_ITEMS(name, items, N)                                            \
  do {                                                                         \
    printf("%s\n", name);                                                      \
    for (int i = 0; i < N; i++) {                                              \
      item_t t = items[i];                                                     \
      printf("%d %d %d\n", t.cost, t.damage, t.armor);                         \
    }                                                                          \
  } while (false)

typedef struct item {
  int cost;
  int damage;
  int armor;
} item_t;

typedef struct shop {
  item_t weapons[N_WEAPON];
  item_t armors[N_ARMOR];
  item_t rings[N_RINGS];
} shop_t;

typedef struct player {
  int health;
  int damage;
  int armor;
  int cost;
} player_t;

int max(int a, int b) { return a > b ? a : b; }

int divide_ceil(int a, int b) {
  int result = a / b;

  if (a % b != 0) {
    result += 1;
  }

  return result;
}

void set_item(int cost, int damage, int armor, item_t *item) {
  item->cost = cost;
  item->damage = damage;
  item->armor = armor;
}

player_t *create_player(int health, int damage, int armor, int cost) {
  player_t *b = malloc(sizeof(player_t));
  b->health = health;
  b->damage = damage;
  b->armor = armor;
  b->cost = cost;
  return b;
}

player_t *create_boss(int health, int damage, int armor) {
  return create_player(health, damage, armor, 0);
}

shop_t *get_shop() {
  shop_t *shop = malloc(sizeof(shop_t));

  int weapons[N_WEAPON * 3] = {
      8,  4, 0, // Dagger
      10, 5, 0, // Shortsword
      25, 6, 0, // Warhammer
      40, 7, 0, // Longsword
      74, 8, 0, // Longsword
  };

  int armors[N_ARMOR * 3] = {
      0,   0, 0, // EMPTY
      13,  0, 1, // Leather
      31,  0, 2, // Chainmail
      53,  0, 3, // Splintmail
      75,  0, 4, // Bandemail
      102, 0, 5, // Plateamil
  };

  int rings[N_RINGS * 3] = {
      0,   0, 0, // EMPTY
      25,  1, 0, // Damage +1
      50,  2, 0, // Damage +2
      100, 3, 0, // Damage +3
      20,  0, 1, // Defense +1
      40,  0, 2, // Defense +2
      80,  0, 3, // Defense +3
  };

  SET_ITEMS(&shop->weapons, weapons, N_WEAPON);
  SET_ITEMS(&shop->armors, armors, N_ARMOR);
  SET_ITEMS(&shop->rings, rings, N_RINGS);

  return shop;
}

int cmp(const void *a, const void *b) {
  return (*(player_t **)a)->cost - (*(player_t **)b)->cost;
}

player_t **get_all_players(int health, shop_t *shop, int *out) {
  int size = 0;
  player_t **players = NULL;

  for (int i = 0; i < N_WEAPON; i++) {
    for (int j = 0; j < N_ARMOR; j++) {
      for (int k = 0; k < N_RINGS; k++) {
        for (int l = 0; l < N_RINGS; l++) {
          if (shop->rings[k].cost != 0 && shop->rings[k].cost != 0 && k == l) {
            // can't equip the same ring twice
            continue;
          }

          item_t w = shop->weapons[i];
          item_t a = shop->armors[j];
          item_t lr = shop->rings[k];
          item_t rr = shop->rings[l];

          int damage = w.damage + lr.damage + rr.damage;
          int armor = a.armor + lr.armor + rr.armor;
          int cost = w.cost + a.cost + lr.cost + rr.cost;

          players = realloc(players, sizeof(player_t *) * (size + 1));
          players[size++] = create_player(health, damage, armor, cost);
        }
      }
    }
  }

  *out = size;
  qsort(players, size, sizeof(player_t *), cmp);

  return players;
}

void free_players(player_t **players, int size) {
  for (int i = 0; i < size; i++) {
    free(players[i]);
  }

  free(players);
}

bool fight(player_t *p, player_t *b) {
  int boss_hits = divide_ceil(p->health, max(1, b->damage - p->armor));
  int player_hits = divide_ceil(b->health, max(1, p->damage - b->armor));

  return player_hits <= boss_hits;
}

int main() {
  shop_t *shop = get_shop();
  player_t *boss = create_boss(109, 8, 2);

  int size = 0;
  player_t **players = get_all_players(100, shop, &size);

  int part_1 = -1;
  int part_2 = -1;

  for (int i = 0; i < size; i++) {
    player_t *a = players[i];
    player_t *b = players[size - i - 1];

    if (part_1 == -1 && fight(a, boss)) {
      part_1 = a->cost;
      continue;
    }

    if (part_2 == -1 && !fight(b, boss)) {
      part_2 = b->cost;
      continue;
    }

    if (part_1 != -1 && part_2 != -1) {
      break;
    }
  }

  printf("%d\n", part_1);
  printf("%d\n", part_2);

  free_players(players, size);
  free(boss);
  free(shop);

  return 0;
}
