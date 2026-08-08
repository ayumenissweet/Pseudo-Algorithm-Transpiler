#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    char* key;
    float value;
} MapEntry;

typedef struct Node {
    char* key;
    float value;
    struct Node* next;
} Node;

typedef struct {
    Node** heads;
    int length; 
    int loadFactor; 
} Hashmap;

typedef struct {
    Hashmap *map;
    Node* current;
    int head;
} Iterator;

Hashmap initMap();
float* get(Hashmap map, const char* key);
void set(Hashmap* map, const char* key, float value);
void removeMap(Hashmap *map, const char* key);
bool containsKey(Hashmap map, const char* key);
bool containsValue(Hashmap map, float value);
int size(Hashmap map);
void displayMap(Hashmap map);
void freeMap(Hashmap *map);
Hashmap initMapArray_impl(MapEntry *entries, size_t count);
Iterator iterator_init(Hashmap* map);
Node* iter_next(Iterator* iter);

#define initMapOf(...) \
    (sizeof((MapEntry[]){__VA_ARGS__}) == sizeof(MapEntry) && #__VA_ARGS__[0] == '\0') ? \
    initMapArray_impl(NULL, 0) : \
    initMapArray_impl((MapEntry[]){__VA_ARGS__}, sizeof((MapEntry[]){__VA_ARGS__}) / sizeof(MapEntry))

#endif
