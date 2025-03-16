#pragma once
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>



typedef struct MapNode {
    char key[100]; // TODO no magic;
    size_t values[10000];
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
size_t      _isInHashmap(Hashmap* map, MapNode* node, const char* key);
void        freeHashmap(Hashmap* map);
size_t      getValueFromHashmap(Hashmap* map, const char* key);
size_t      _getValueFromHashmap(Hashmap* map, MapNode* node, const char* key);
void        resetCursor(Hashmap* map, const char* key);
void        _tryInsert(Hashmap* map, const char* key, size_t value, MapNode* node);

unsigned int hash(const char *key, size_t table_size);