#pragma once
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    char key[100];
    size_t value;
    size_t obs;
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
size_t getValueFromHashmap(Hashmap* map, const char* key);

unsigned int hash(const char *key, size_t table_size);