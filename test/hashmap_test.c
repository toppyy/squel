#include "../src/util/hashmap.c"
#include <stdio.h>

int main() {

    Hashmap* map = initHashmap(1000);

    insertToHashmap(map, "12345", 1442);
    insertToHashmap(map, "12346", 2);
    insertToHashmap(map, "12X46", 3);

    if (isInHashmap(map, "12345")) {
        printf("12345 in map with value %ld\n", getValueFromHashmap(map, "12345"));
    }

    if (isInHashmap(map, "12X46")) {
        printf("12X46 in map\n");
    }

    if (isInHashmap(map, "123fASFA")) {
        printf("12X46 in map\n");
    }


    freeHashmap(map);

    return 0;
}