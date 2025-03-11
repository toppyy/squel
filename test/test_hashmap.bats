
#!/usr/bin/env bats

setup_file() {
    run rm ./build/hashmap_test.o
    run gcc ./test/hashmap_test.c -o ./build/hashmap_test.o
}

@test "Hashmap functionality" {
    run ./build/hashmap_test.o
    [[ $"${lines[0]}" == "12345 in map with value 1442" ]]
    [[ $"${lines[1]}" == "12X46 in map" ]]
}

