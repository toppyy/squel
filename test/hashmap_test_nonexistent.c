#include "../src/util/hashmap.c"
#include <stdio.h>

int main() {

    Hashmap* map = initHashmap(1000);

    insertToHashmap(map, "key1", 100);
    insertToHashmap(map, "key2", 200);

    if (!isInHashmap(map, "nonexistent")) {
        printf("nonexistent key not in map\n");
    }

    size_t val = getValueFromHashmap(map, "nonexistent");
    if (val == 0) {
        printf("getValue for nonexistent key returns 0\n");
    }

    freeHashmap(map);

    return 0;
}
