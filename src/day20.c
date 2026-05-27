#include <math.h>
#include <stdint.h>
#include <stdio.h>

uint64_t compute_house(uint64_t house, uint64_t start,
                       uint64_t present_per_house) {
  if (house <= 0) {
    return 0;
  }

  uint64_t presents = 0;

  for (uint64_t i = 1; i <= sqrt(house); i++) {
    if (house % i == 0) {
      if (i >= start) {
        presents += (i * present_per_house);
      }
      if (house / i != i) {
        if (house / i >= start) {
          presents += (house / i) * present_per_house;
        }
      }
    }
  }

  return presents;
}

uint64_t solve(uint64_t target, uint64_t max_house_per_elf,
               uint64_t presents_per_house) {
  uint64_t house = 1;

  while (true) {
    uint64_t start = 1;

    if (max_house_per_elf > 0) {
      // For part 2, compute which first elf will
      // visit the current house based on how many
      // houses each elf will visit. Skipping this
      // branch means an elf will visit an infinite
      // amout of houses, aka doing part 1
      start = house / max_house_per_elf;
      start = house % max_house_per_elf != 0 ? start + 1 : start;
    }

    uint64_t presents = compute_house(house, start, presents_per_house);

    if (presents >= target) {
      return house;
    }

    house++;
  }
}

int solve_p1(int target) {
  int max_house_per_elf = 0; // infinite amount of houses
  int presents_per_house = 10;
  return solve(target, max_house_per_elf, presents_per_house);
}

uint64_t solve_p2(uint64_t target) {
  int max_house_per_elf = 50;
  int presents_per_house = 11;
  return solve(target, max_house_per_elf, presents_per_house);
}

int main() {
  uint64_t input = 33100000;

  int part_1 = solve_p1(input);
  printf("%d\n", part_1);

  int part_2 = solve_p2(input);
  printf("%d\n", part_2);

  return 0;
}
