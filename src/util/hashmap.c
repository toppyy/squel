#include "../include/util/hashmap.h"

Hashmap* initHashmap(size_t table_size) {
    Hashmap* map = malloc(sizeof(Hashmap));
    map->data = calloc(table_size, sizeof(MapNode));
    if (map->data == NULL) {
        printf("Error: unable to reserve %ld bytes\n", (sizeof(MapNode) * table_size) / 1024);
        exit(1);
    }
    map->table_size = table_size;
    return map;
}

void insertToHashmap(Hashmap* map, const char* key, size_t value) {
    unsigned int idx = hash(key, map->table_size);

    MapNode* node = &map->data[idx];
    _tryInsert(map, key, value, node);
}



void _tryInsert(Hashmap* map __attribute__((unused)), const char* key, size_t value, MapNode* node) {
    
    if (strlen(key) == 0) return; // Where do these come from?

    if (node->obs == 0) {

        memcpy(node->key, key, strlen(key));

    } else {

        if (strcmp(key, node->key) != 0) {
            if (!node->next) {
                node->next = calloc(1, sizeof(MapNode));
            }

            _tryInsert(map, key, value, node->next);
            return;
        }
    }
    node->values[node->obs] = value;
    
    if (node->obs >= 10000) {
        // printf("OUT OF BOUNDS\n"); // TODOs
        return;
    }
    node->obs++;
}

size_t _isInHashmap(Hashmap* map, MapNode* node, const char* key) {
    if (strcmp(key, node->key) != 0) {
        if (!node->next) {
            return 0;
        }
        return _isInHashmap(map, node->next, key);
    }
    if (node->cursor == node->obs) return 0;
    return node->obs > 0 ? 1 : 0;
}


size_t isInHashmap(Hashmap* map, const char* key) {
    unsigned int idx = hash(key, map->table_size);
    MapNode* node = &map->data[idx];
    return _isInHashmap(map, node, key);
}


void resetCursor(Hashmap* map, const char* key) {
    unsigned int idx = hash(key, map->table_size);
    map->data[idx].cursor = 0;
}

size_t getValueFromHashmap(Hashmap* map, const char* key) {
    unsigned int idx = hash(key, map->table_size);
    MapNode* node = &map->data[idx];
    return _getValueFromHashmap(map, node, key);
}

size_t _getValueFromHashmap(Hashmap* map, MapNode* node, const char* key) {

    if (strcmp(key, node->key) != 0) {
        if (!node->next) {
            return 0;
        }

        return _getValueFromHashmap(map, node->next, key);
    }

    if (node->cursor == node->obs)  return -1;

    return node->values[node->cursor++];

}

void freeHashMapNode(MapNode* node) {
    if (!node) return;

    if (node->next) {
        freeHashMapNode(node->next);
    }
    free(node);
}

void freeHashmap(Hashmap* map) {
    MapNode* node;
    for (size_t i = 0; i < map->table_size; i++) {
        node = &map->data[i];

        if (node == 0) break;

        freeHashMapNode(node->next); // Only adjacents need to be freed
        
    }
    free(map->data);
    free(map);
}

unsigned int hash(const char *key, size_t table_size) {
    unsigned long int hashval = 0;
    while (*key) {
        hashval = (hashval << 5) + *key++;
    }
    return hashval % table_size;
}