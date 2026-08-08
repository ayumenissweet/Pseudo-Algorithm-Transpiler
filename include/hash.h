#ifndef HASH_H
#define HASH_H

#include <stdint.h>

uint64_t hash(const char *str);

int getRandomIndex(uint64_t hash, int size);

#endif
