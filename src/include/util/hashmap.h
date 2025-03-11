#pragma once
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    char key[100]; // TODO no magic;
    size_t values[1000];
    size_t obs;
    size_t cursor;
    struct MapNode* next;
} MapNode;


typedef struct  {
    MapNode* data;
    size_t table_size;
} Hashmap;


Hashmap*    initHashmap(size_t table_size);
void        insertToHashmap(Hashmap* map, const char* key, size_t value);
size_t      isInHashmap(Hashmap* map, const char* value);
void        freeHashmap(Hashmap* map);
size_t      getValueFromHashmap(Hashmap* map, const char* key);
void        resetCursor(Hashmap* map, const char* key);

unsigned int hash(const char *key, size_t table_size);