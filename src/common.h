#ifndef COMMON_H
#define COMMON_H

#include <regex.h>
#include <stdio.h>

FILE *fopen_orexit(char *);
char *fopen_string_orexit(char *, int *);
void regcomp_orexit(regex_t *, char *, int);

int min2(int, int);
int min3(int, int, int);

int max2(int, int);

#endif // COMMON_H
