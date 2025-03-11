#include "../include/util/hashmap.h"



Hashmap* initHashmap(size_t table_size) {
    Hashmap* map = malloc(sizeof(Hashmap));
    map->data = calloc(table_size, sizeof(MapNode));
    map->table_size = table_size;
    return map;
}

void insertToHashmap(Hashmap* map, const char* key, size_t value) {
    unsigned int idx = hash(key, map->table_size);
    if (map->data[idx].obs == 0) {
        memcpy(map->data[idx].key, key, strlen(key));
    }
    map->data[idx].value = value;
    map->data[idx].obs++;
    // TODO handle collisions
}

size_t isInHashmap(Hashmap* map, const char* key) {
    unsigned int idx = hash(key, map->table_size);
    return map->data[idx].obs > 0 ? 1 : 0;
}

size_t getValueFromHashmap(Hashmap* map, const char* key) {
    unsigned int idx = hash(key, map->table_size);
    return map->data[idx].value;
}




void freeHashmap(Hashmap* map) {
    free(map->data); // TODO free any adjacent nodes after handling collitions
    free(map);
}

unsigned int hash(const char *key, size_t table_size) {
    unsigned long int hashval = 0;
    while (*key) {
        hashval = (hashval << 5) + *key++;
    }
    return hashval % table_size;
}