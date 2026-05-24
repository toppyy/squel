#include "../src/util/hashmap.c"
#include <stdio.h>

int main() {

    Hashmap* map = initHashmap(1000);

    insertToHashmap(map, "key1", 100);
    insertToHashmap(map, "key1", 200);

    size_t val1 = getValueFromHashmap(map, "key1");
    printf("first value: %ld\n", val1);

    size_t val2 = getValueFromHashmap(map, "key1");
    printf("second value: %ld\n", val2);

    resetCursor(map, "key1");
    size_t val3 = getValueFromHashmap(map, "key1");
    printf("after reset: %ld\n", val3);

    freeHashmap(map);

    return 0;
}
