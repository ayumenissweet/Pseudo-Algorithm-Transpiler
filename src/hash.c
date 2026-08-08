#include "../include/hash.h"

uint64_t hash(const char *str){
    uint64_t hash = 5381;
    int c;
    while((c = *str++)){
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

int getRandomIndex(uint64_t hash, int size){
    return hash & (size - 1); // requires size to be a power of two
}
